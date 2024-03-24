import re
import os
import os.path
import clang.cindex
import traceback
import sys
import time
import multiprocessing
import tempfile
from config import ENABLE_MULTIPROCESSING, ENABLE_UNITY_BUILD, VERBOSE_TIMINGS, VERBOSE

PATTERN_DISCARD_FROM_UNITY_BUILD = re.compile(r'LIGHTEN_REFLECTION_DISCARD_FILE_FROM_UNITY_BUILD')


def fully_qualified(c):
    if c is None:
        return ''
    elif c.kind == clang.cindex.CursorKind.TRANSLATION_UNIT:
        return ''
    else:
        res = fully_qualified(c.semantic_parent)
        if res != '':
            return res + '::' + c.spelling
    return c.spelling

def format_diagnostics(translation_unit, file):
    nb_diag = translation_unit.diagnostics
    if len(nb_diag) == 0:
        return (False, "")
    out_str = f"There are {len(nb_diag)} diagnostics for {file}:\n"

    found_error = False
    for diagnostic in nb_diag:
        error_string = str(diagnostic)
        if diagnostic.severity >= clang.cindex.Diagnostic.Error:
            found_error = True
        out_str += error_string + "\n"

    if found_error:
        out_str += "There are errors in the file. Stopping reflection generator.\n"
    return (found_error, out_str)

def parse_settings_to_dict(parameter_tokens):
    settings = {}
    current_key = None
    current_value = ""
    is_value = False
    end = len(parameter_tokens)
    for i, token in enumerate(parameter_tokens[::-1]):
        if token.spelling == ')':
            end = len(parameter_tokens) - i - 1
            break
    for token in parameter_tokens[:end]:
        token_str = token.spelling
        if is_value:
            if token_str in [',', ')']:
                settings[current_key] = current_value.strip().strip('"')
                current_key = None
                current_value = ""
                is_value = False
            else:
                current_value += (' ' + token_str if current_value else token_str)
        else:
            if token_str == '=':
                is_value = True
            elif token_str not in [',', '(']:
                if current_key is not None:
                    settings[current_key] = True
                current_key = token_str

    # Handle the last key if it doesn't have a value
    if current_key is not None and not is_value:
        settings[current_key] = True
    if current_key is not None and is_value:
        settings[current_key] = current_value.strip().strip('"')

    # Convert boolean and numeric values
    for key in list(settings):
        value = settings[key]
        if isinstance(value, str):
            if value.lower() in ["true", "false"]:
                settings[key] = value.lower() == "true"
            elif value.isdigit():
                settings[key] = int(value)
            elif value.replace('.', '', 1).isdigit() and value.count('.') < 2:
                settings[key] = float(value)

    return settings


def find_preceding_annotation_and_settings(parent, start_location, end_location, relevant_tokens):
    start_location = parent.extent.start if start_location is None else start_location
    search_range = clang.cindex.SourceRange.from_locations(
        start_location, end_location
    )

    annotation = None
    settings = None
    location = None
    for token in parent.translation_unit.get_tokens(extent=search_range):
        token_str = token.spelling
        if token_str in relevant_tokens and (start_location is None or token.extent.start != start_location):
            annotation = token_str.split('(')[0].strip()
            settings = list(parent.translation_unit.get_tokens(extent=clang.cindex.SourceRange.from_locations(token.extent.start, end_location)))[1:-1]
            settings = parse_settings_to_dict(settings)
            location = token.extent.start
            break  # Assuming the relevant macro is the last one before the class/struct declaration

    return annotation, settings, location

def find_properties_and_methods(node):
    properties = {}
    methods = {}
    getters = {}
    setters = {}

    last_seen_location = None
    last_seen_location = None

    for child in node.get_children():
        # Handle properties
        if child.kind == clang.cindex.CursorKind.FIELD_DECL:
            end = next(child.get_children(), None)
            end_location = end.extent.start if end else child.extent.start
            end_location = end_location if end_location.offset < child.extent.start.offset else child.extent.start
            annotation, settings, location = find_preceding_annotation_and_settings(node, last_seen_location, end_location, ["LIGHTEN_PROPERTY"])
            if (last_seen_location is None and location is not None) or (location is not None and location != last_seen_location):
                properties[child.spelling] = {
                    "settings": settings,
                    "name": child.spelling, 
                    "type": child.type.get_canonical().spelling,
                    "fully_qualified_name": fully_qualified(child)
                }
                last_seen_location = location      
        elif child.kind in [clang.cindex.CursorKind.CXX_METHOD, clang.cindex.CursorKind.FUNCTION_TEMPLATE]:
            end = next(child.get_children(), None)
            end_location = end.extent.start if end else child.extent.start
            end_location = end_location if end_location.offset < child.extent.start.offset else child.extent.start
            annotation, settings, location = find_preceding_annotation_and_settings(node, last_seen_location, end_location, ["LIGHTEN_METHOD", "LIGHTEN_PROPERTY_GETTER", "LIGHTEN_PROPERTY_SETTER"])
            if (last_seen_location is None and location is not None) or (location is not None and location != last_seen_location):
                method_name = child.spelling
                method_return_type = child.result_type.spelling
                is_const = None
                try:
                    is_const = child.is_const_method()
                except:
                    pass
                parameters = {}
                for arg in child.get_arguments():
                    parameters[arg.spelling] = arg.type.get_canonical().spelling

                method_info = {
                    "name": method_name, 
                    "return_type": method_return_type, 
                    "fully_qualified_name": fully_qualified(child),
                    "settings": settings,
                    "is_const": is_const,
                    "parameters": parameters
                }

                # Check if it's a getter or setter
                if annotation == "LIGHTEN_PROPERTY_GETTER":
                    getters[method_info["name"]] = method_info
                elif annotation == "LIGHTEN_PROPERTY_SETTER":
                    setters[method_info["name"]] = method_info
                else:
                    methods[method_info["name"]] = method_info

                last_seen_location = location


    return properties, methods, getters, setters

def find_info(parent, whitelist, parent_namespace=''):
    last_seen_class_location = None
    rv_data = {}
    skipped_files = []
    for node in parent.get_children():
        if os.path.normpath(node.location.file.name) not in whitelist:
            if VERBOSE and node.location.file.name not in skipped_files:
                print(f"Skipping file: {node.location.file.name}")
                skipped_files.append(node.location.file.name)
            continue
        try:
            if node.kind in [clang.cindex.CursorKind.CLASS_DECL, clang.cindex.CursorKind.STRUCT_DECL]:
                end = next(node.get_children(), None)
                end_location = end.extent.start if end else node.extent.start
                end_location = end_location if end_location.offset < node.extent.start.offset else node.extent.start
                annotation, settings, location = find_preceding_annotation_and_settings(parent, last_seen_class_location, end_location, ["LIGHTEN_DATACLASS", "LIGHTEN_COMPONENT", "LIGHTEN_SYSTEM"])
                if (
                    (last_seen_class_location is None and location is not None) or (location is not None and location != last_seen_class_location)
                ):
                    properties, methods, getters, setters = find_properties_and_methods(node)
                    rv_data[node.spelling] = {
                        "name": node.spelling,
                        "namespace": parent_namespace,
                        "fully_qualified_name": fully_qualified(node),
                        "settings": settings,
                        "properties": properties,
                        "methods": methods,
                        "getters": getters,
                        "setters": setters,
                        "kind": annotation,
                        "file": node.location.file.name
                    }
                    last_seen_class_location = location
                    if VERBOSE:
                        print(f"{annotation} with settings: {settings}")
                        print(f"Class/Struct: {node.spelling}, Namespace: {parent_namespace}")
                        for property in properties.values():
                            print(f"  Property: {property['name']}, Type: {property['type']}, settings: {property['settings']}")
                        for method in methods:
                            print(f"  Method: {method['name']}, Return type: {method['return_type']}, settings: {method['settings']}")
                        for getter in getters:
                            print(f"  Getter: {getter['name']}, Return type: {getter['return_type']}, settings: {getter['settings']}")
                        for setter in setters:
                            print(f"  Setter: {setter['name']}, Return type: {setter['return_type']}, settings: {setter['settings']}")
                    
                    for property in properties.values():
                        if property['settings'].get('serialize', False):
                            rv_data[node.spelling]['settings']['serialize'] = True
                        if property['settings'].get('save_game', False):
                            rv_data[node.spelling]['settings']['save_game'] = True
            elif node.kind == clang.cindex.CursorKind.NAMESPACE:
                new_namespace = f"{parent_namespace}::{node.spelling}" if parent_namespace else node.spelling
                rv_data.update(find_info(node, whitelist, new_namespace))
        except Exception as e:
            if "Unknown template argument kind 604" in str(e):
                return
            print(traceback.format_exc())
    return rv_data

def process_file(args):
    rv_data = {}
    try:
        file, include_dirs, whitelist = args
        index = clang.cindex.Index.create()
        start_time = time.time()
        tu = index.parse(file, args=['-std=c++20'] + [f'-I{include_dir}' for include_dir in include_dirs])
        duration = time.time() - start_time
        out_str = f"Translation unit made in {duration:.3f}s: {file}\n"
        error, diagnostics_str = format_diagnostics(tu, file)
        out_str += diagnostics_str
        start_time = time.time()
        rv_data.update(find_info(tu.cursor, whitelist))
        duration = time.time() - start_time
        out_str += f"Python Processing Time: {duration:.3f}"
        if VERBOSE_TIMINGS or error:
            print(out_str)
            if error:
                exit(-1)
    except:
        print(traceback.format_exc())
        return (-1, rv_data)
    return (0, rv_data)

def create_unity_file(source_files, unity_file_path):
    with open(unity_file_path, 'w') as unity_file:
        if VERBOSE_TIMINGS:
            print(f"Creating unity file: {unity_file_path} with: ")
        for file in source_files:
            if VERBOSE_TIMINGS:
                print(f"  {file}")
            unity_file.write(f'#include "{file}"\n')

def should_exclude_from_unity_build(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    return PATTERN_DISCARD_FROM_UNITY_BUILD.search(content) is not None



def process_files(include_dirs, source_files):
    if len(source_files) == 0:
        return {}

    excluded_files = [file for file in source_files if should_exclude_from_unity_build(file)]
    included_files = [file for file in source_files if file not in excluded_files]

    unity_file_path = None
    if ENABLE_UNITY_BUILD and included_files:
        unity_file_path = os.path.join(tempfile.gettempdir(), "unity_build.cpp")
        create_unity_file(included_files, unity_file_path)

    files_to_process = []
    whitelist = []

    # Create a whitelist for Unity build
    if unity_file_path:
        files_to_process.append(unity_file_path)
        whitelist.append(included_files)  # Add all included files to the whitelist

    # Add excluded files and their individual whitelists
    for file in excluded_files:
        files_to_process.append(file)
        whitelist.append([file])  # The file's whitelist contains only itself
    whitelist = [[os.path.normpath(file) for file in sublist] for sublist in whitelist]
    output = [0, {}]
    if ENABLE_MULTIPROCESSING and len(files_to_process) > 1:
        with multiprocessing.Pool() as pool:
            file_args = [(file, include_dirs, wl) for file, wl in zip(files_to_process, whitelist)]
            results = pool.map(process_file, file_args)
            output[0] += sum(i[0] for i in results)
            output[1].update(i[1] for i in results)
    else:
        for file, wl in zip(files_to_process, whitelist):
            result = process_file((file, include_dirs, wl))
            output[0] += result[0]
            output[1].update(result[1])


    if unity_file_path and os.path.exists(unity_file_path):
        os.remove(unity_file_path)
    if output[0] < 0: # Some fatal error occured
        sys.exit(output[0])
    return output[1]
