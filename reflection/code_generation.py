import os
import os.path
import yaml
import config
import string

# Since Windows does not support symlinks, we need to use hardlinks instead
# Only administrators can create symbolic links on Windows:
# https://stackoverflow.com/questions/6260149/os-symlink-support-in-windows
def create_hard_link(source, link_name):
    """
    Creates a hard link pointing to source named link_name.
    
    Parameters:
        source (str): The path to the source file.
        link_name (str): The path for the hard link.
    """
    try:
        # Remove the link if it already exists
        if os.path.exists(link_name):
            os.remove(link_name)
        
        os.link(source, link_name)
        if config.VERBOSE:
            print(f"Successfully created hard link: {link_name} -> {source}")
    except OSError as e:
        print(f"Could not create hard link: {e}")


SERIALIZATION_FUNCTION_TEMPLATE = string.Template("""    template<class Archive>
    void serialize(Archive& archive, ::$namespace::$class_name& t) {
$properties
    }""")

SERIALIZATION_PROPERTY_TEMPLATE = string.Template("        archive(cereal::make_nvp(\"$prop_name\", t.$prop_name));")


REFLECTION_PREDEFINED_STRUCTS = """
    // Compile-time reflection metadata for components
    struct ComponentMetadata {
        consteval ComponentMetadata(
            const char* const name,
            const char* const category,
            bool serialize,
            bool save_game,
            bool editor_hide,
            bool editor_readonly
        ) : name(name), 
            category(category), 
            serialize(serialize), 
            save_game(save_game), 
            editor_hide(editor_hide), 
            editor_readonly(editor_readonly) {}
            
        const char* const name;
        const char* const category;
        const bool serialize;
        const bool save_game;
        const bool editor_hide;
        const bool editor_readonly;
    };

    // Compile-time reflection metadata for systems
    struct SystemMetadata
    {
        consteval SystemMetadata(
            const char* const name,
            const char* const category) : name(name), category(category) {}
        const char* const name;
        const char* const category;
    };
"""

COMPONENT_METADATA = string.Template('        ComponentMetadata { "$name", "$category", $serialize, $save_game, $editor_hide, $editor_readonly },')
SYSTEM_METADATA = string.Template('        SystemMetadata { "$name", "$category" },')

def generate_hpp_serialization_code(global_data):
    includes = {
        "#include <cereal/cereal.hpp>",
        "#include <variant>",
        "#include <string>",
        "#include <array>"
    }

    serialization_code_parts = []
    serialization_functions = []

    component_variant_strings = []
    component_metadata_list = []

    system_variant_strings = []
    system_metadata_list = []


    for _, components in global_data.items():
        for class_name, class_info in components.items():
            include = class_info['file'].replace('\\', '/')
            includes.add(f"#include \"{include}\"")

            # Generate serialization for properties
            properties_serialization = "\n".join(
                SERIALIZATION_PROPERTY_TEMPLATE.substitute(prop_name=prop_name)
                for prop_name in class_info.get("properties", {})
            )
            if class_info['kind'] == 'LIGHTEN_COMPONENT' or class_info['kind'] == 'LIGHTEN_DATACLASS':
                serialization_functions.append(SERIALIZATION_FUNCTION_TEMPLATE.substitute(
                    namespace=class_info['namespace'],
                    class_name=class_name,
                    properties=properties_serialization
                ))
            
            if class_info['kind'] == 'LIGHTEN_COMPONENT':
                component_variant_strings.append(f"        ::{class_info['namespace']}::{class_name}")
                component_metadata_list.append(COMPONENT_METADATA.substitute(
                    name=class_info['settings'].get('name', class_name),
                    category=class_info['settings'].get('category', "Default"),
                    serialize=str(class_info['settings'].get('serialize', 'false')).lower(),
                    save_game=str(class_info['settings'].get('save_game', 'false')).lower(),
                    editor_hide=str(class_info['settings'].get('editor_hide', 'false')).lower(),
                    editor_readonly=str(class_info['settings'].get('editor_readonly', 'false')).lower()
                ))
            elif class_info['kind'] == 'LIGHTEN_SYSTEM':
                system_variant_strings.append(f"        ::{class_info['namespace']}::{class_name}")
                system_metadata_list.append(SYSTEM_METADATA.substitute(
                    name=class_info['settings'].get('name', class_name),
                    category=class_info['settings'].get('category', "Default")
                ))
                
    serialization_code_parts.append("\n".join(sorted(includes)))
    serialization_code_parts.append("namespace lighten::serialization {")
    serialization_code_parts.append(REFLECTION_PREDEFINED_STRUCTS)
    serialization_code_parts.append("    using ComponentVariant = std::variant<")
    serialization_code_parts.append(",\n".join(component_variant_strings))
    serialization_code_parts.append("    >;")
    serialization_code_parts.append("    constexpr std::array<ComponentMetadata, " + str(len(component_metadata_list)) + "> component_metadata = {")
    serialization_code_parts.extend(component_metadata_list)
    serialization_code_parts.append("    };")
    serialization_code_parts.append("    using SystemVariant = std::variant<")
    serialization_code_parts.append(",\n".join(system_variant_strings))
    serialization_code_parts.append("    >;")
    serialization_code_parts.append("    constexpr std::array<SystemMetadata, " + str(len(system_metadata_list)) + "> system_metadata = {")
    serialization_code_parts.extend(system_metadata_list)
    serialization_code_parts.append("    };")
    serialization_code_parts.append("} // namespace lighten::serialization")
    serialization_code_parts.append("namespace cereal {")
    serialization_code_parts.extend(serialization_functions)
    serialization_code_parts.append("}// namespace cereal")

    return "\n".join(serialization_code_parts)

GUI_DECL_TEMPLATE = string.Template("""
    template<>
    struct ComponentWidget {
        inline void Draw($namespace::$class_name const &t, std::string_view const name) {
            if (ImGui::TreeNode(name.data())) {
#if defined(LIGHTEN_FLEXIBLE_GUI)
$children_calls
#else
$widget_calls
#endif
                ImGui::TreePop();
            }
        }  
#if defined(LIGHTEN_FLEXIBLE_GUI)
        // Allows live updates of the GUI when editing YAML files.
        const char *file_path_to_yaml_file = "$yaml_file_path";
        
        // Should be updated when the YAML file is updated.
        std::string rendering_data = "";

        // data generated by updating yaml
        std::unordered_map<std::string, std::unique_ptr<BaseWidget>> children;
#else
        // Locked state generated by YAML files.
$widgets
#endif
    };
""")
GUI_DEFN_CHILDREN_CALLS_TEMPLATE = string.Template('                children["$prop_name"]->DrawPtr((void*)(&t.$prop_name), \"$prop_visual_name\");')
GUI_DEFN_WIDGET_CALLS_TEMPLATE = string.Template("                  $prop_name.Draw(t.$prop_name, \"$prop_visual_name\");")

def generate_hpp_gui_code(data, code_dir):
    includes = []
    for component_name, component_data in data.items():
        include = component_data['file'].replace('\\', '/')
        includes.append(f"#include \"{include}\"")
    includes = list(set(includes))
    
    includes = ["#include <gui/gui-core.hpp>"] + includes
    
    gui_code = "\n".join(includes) + "\nnamespace lighten::gui {\n"
    for component_name, component_data in data.items():
        # Generate YAML file path for each component
        file_name = component_data['namespace'].replace("::", "_") + "_" + component_name + ".yaml"
        file_path = os.path.join(code_dir, "gui/yaml", file_name).replace('\\', '/')
        children_calls = "\n".join(GUI_DEFN_CHILDREN_CALLS_TEMPLATE.substitute(type=prop_data["type"], prop_name=prop_name, prop_visual_name=prop_data["settings"]["name"]) for prop_name, prop_data in component_data.get("properties", {}).items())
        widget_calls = "\n".join(GUI_DEFN_WIDGET_CALLS_TEMPLATE.substitute(prop_name=prop_name, prop_visual_name=prop_data["settings"]["name"]) for prop_name, prop_data in component_data.get("properties", {}).items())
   
        gui_code += GUI_DECL_TEMPLATE.substitute(
                        namespace=component_data['namespace'], 
                        class_name=component_name, 
                        name=component_data['settings']['name'],
                        yaml_file_path = file_path,
                        children_calls = children_calls,
                        widget_calls = widget_calls,
                        widgets = "\n".join(f"        Widget<{prop_data['type']}> {prop_name};" for prop_name, prop_data in component_data.get("properties", {}).items())
        )

    gui_code += "} // namespace lighten::gui\n"
    return gui_code


def update_yaml_files(data, code_dir, runtime_dir, build_dir):
    index = {}
    index["ui_files"] = []
    for component_name, component_data in data.items():
        yaml_data = {}

        yaml_data["name"] = component_data["settings"]["name"]
        yaml_data["category"] = component_data["settings"]["category"]
        # Generate YAML file path for each component
        file_name = component_data['namespace'].replace("::", "_") + "_" + component_name + ".yaml"
        file_path = os.path.join(build_dir, "gui/yaml", file_name)
        index["ui_files"].append(file_name)
        
        # Read existing YAML file if it exists
        if os.path.exists(file_path):
            try:
                with open(file_path, 'r') as file:
                    existing_data = yaml.safe_load(file) or {}
            except:
                existing_data = {}
        else:
            existing_data = {}
        
 #       create_hard_link(file_path, os.path.join(runtime_dir, "gui/yaml", file_name))
            
        for category in ["properties", "getters", "setters", "methods"]:
            if category in existing_data:
                if existing_data[category] is None:
                    existing_data[category] = {}
                items_to_delete = [item_name for item_name in existing_data[category].keys()
                                if item_name not in component_data.get(category, {})]
                for item_name in items_to_delete:
                    del existing_data[category][item_name]

        for category in ["properties", "getters", "setters", "methods"]:
            yaml_data[category] = {}
            for item_name, item_data in component_data.get(category, {}).items():
                # Check if name and type of properties match, if not, overwrite completely
                if category == "properties" and (existing_data.get(category, {}).get(item_name, {}).get('type', None) == item_data.get('type')):
                    yaml_data[category][item_name] = existing_data[category][item_name]
                elif category != "properties" and (existing_data.get(category, {}).get(item_name, {}).get('return_type', None) == item_data.get('return_type')):
                    yaml_data[category][item_name] = existing_data[category][item_name]
                else:
                    yaml_data[category][item_name] = item_data["settings"]
                    if category == "properties":
                        type_hint = item_data["type"]
                        yaml_data[category][item_name]['type'] = type_hint
                    else:
                        type_hint = item_data["return_type"]    
                        yaml_data[category][item_name]['return_type'] = type_hint
        # Merge existing data with new data
        merged_data = {**existing_data, **component_data, **yaml_data}
        data[component_name] = merged_data
        # Write updated data back to YAML file
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, 'w') as file:
            yaml_content = yaml.dump(merged_data, sort_keys=False)
            file.write(yaml_content)
    index["count"] = len(index["ui_files"])
    # Delete YAML files that are not used anymore
    for dir in [build_dir]: #[runtime_dir, code_dir]:
        os.makedirs(os.path.dirname(os.path.join(dir, "gui/yaml/lighten_components__index.yaml")), exist_ok=True)
        for file_name in os.listdir(os.path.join(dir, "gui/yaml")):
            if file_name not in index["ui_files"]:
                os.remove(os.path.join(dir, "gui/yaml", file_name))
    
    
    with open(os.path.join(build_dir, "gui/yaml/lighten_components__index.yaml"), 'w') as file:
        yaml_content = yaml.dump(index, sort_keys=False)
        file.write(yaml_content)
#    create_hard_link(os.path.join(code_dir, "gui/yaml/lighten_components__index.yaml"),
 #                  os.path.join(runtime_dir, "gui/yaml/lighten_components__index.yaml"))

    print(f"YAML configuration files updated in `{build_dir}`.")