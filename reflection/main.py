import argparse
import os
import os.path
import caching
import time
import yaml
import json
import code_extraction
import code_generation
REFLECTION_FOLDER = os.path.dirname(os.path.abspath(__file__))
HEADER_EXTENSIONS = [".h", ".hpp", ".inl", ".hxx", ".hh", ".inc", ".ipp", ".ixx", ".i++", ".h++"]

def main():
    start = time.time()
    parser = argparse.ArgumentParser(description='Reflection generation tool.')
    parser.add_argument('--global_reflection_output_file', help='Output file for global reflection data')
    parser.add_argument('--global_reflection_cache_dir', help='Cache that holds list of all registered components and systems')
    parser.add_argument('--code_dir', help='Code directory')
    parser.add_argument('--include_dirs', help='Include directories (semicolon-separated)')
    parser.add_argument('--source_files', help='Source files (semicolon-separated)')
    parser.add_argument('--runtime_dir', help='Source files (semicolon-separated)')
    args = parser.parse_args()
    include_dirs = args.include_dirs if args.include_dirs else []
    source_files = args.source_files if args.source_files else []
    
    global_reflection_output_file = os.path.normpath(args.global_reflection_output_file)
    global_reflection_cache_dir = os.path.normpath(args.global_reflection_cache_dir)
    os.makedirs(args.global_reflection_cache_dir, exist_ok=True)

    code_dir = os.path.normpath(args.code_dir)
    runtime_dir = os.path.normpath(args.runtime_dir)
    
    include_dirs = [os.path.normpath(path) for path in include_dirs.split(';')]
    source_files = [os.path.normpath(path) for path in source_files.split(';') if os.path.splitext(path)[1] in HEADER_EXTENSIONS]

    global_cache = caching.load_cache(global_reflection_cache_dir)
    code_cache = global_cache.get(code_dir, {})
    
    source_files = caching.filter_processed_files(code_cache, source_files)
    source_files = caching.discard_files(source_files)

    print(f"Files to process: {len(source_files)}")
    print("Preprocessing took {:.3f} seconds".format(time.time() - start))
    EXTRACT = time.time()
    extracted_data = code_extraction.process_files(include_dirs, source_files)
    print("Extraction took {:.3f} seconds".format(time.time() - EXTRACT))
    UI_GEN = time.time()
    
    LIGHTEN_REFLECTION_YAML = os.path.join(args.global_reflection_cache_dir, "lighten_reflection_data.yaml")
    LIGHTEN_REFLECTION_JSON = os.path.join(args.global_reflection_cache_dir, "lighten_reflection_data.json")

    current_data = {}
    if os.path.exists(LIGHTEN_REFLECTION_YAML):
        with open(LIGHTEN_REFLECTION_YAML, "r") as f:
            current_data = yaml.safe_load(f) or {}

    code_generation.update_yaml_files(extracted_data, code_dir, runtime_dir, global_reflection_cache_dir)

    current_data[code_dir] = extracted_data

    with open(LIGHTEN_REFLECTION_YAML, "w") as f:
        yaml.dump(current_data, f, sort_keys=False)
    with open(LIGHTEN_REFLECTION_JSON, "w") as f:
        json.dump(current_data, f, sort_keys=False, indent=4)
        
    with open(global_reflection_output_file, "w") as f:
        f.write(code_generation.generate_hpp_serialization_code(current_data))

#    with open(os.path.join(code_dir, "lighten_reflection_gui.hpp"), "w") as f:
#        f.write(code_generation.generate_hpp_gui_code(data, code_dir))

#    os.makedirs(os.path.join(runtime_dir, "reflection"), exist_ok=True)
#    code_generation.create_hard_link(os.path.join(code_dir, "lighten_reflection_data.yaml"), os.path.join(runtime_dir, "reflection/lighten_reflection_data.yaml"))
#   code_generation.create_hard_link(os.path.join(code_dir, "lighten_reflection_data.json"), os.path.join(runtime_dir, "reflection/lighten_reflection_data.json"))

    print("Code generation took {} seconds".format(time.time() - UI_GEN))
    caching.save_cache(global_reflection_cache_dir, current_data)
    print("Total time: {} seconds".format(time.time() - start))
if __name__ == "__main__":
    main()
