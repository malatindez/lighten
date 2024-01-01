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
    parser.add_argument('--build_dir', help='Build directory')
    parser.add_argument('--include_dirs', help='Include directories (semicolon-separated)')
    parser.add_argument('--source_files', help='Source files (semicolon-separated)')
    args = parser.parse_args()
    include_dirs = args.include_dirs if args.include_dirs else []
    source_files = args.source_files if args.source_files else []
    build_dir = os.path.normpath(args.build_dir)
    include_dirs = [os.path.normpath(path) for path in include_dirs.split(';')]
    source_files = [os.path.normpath(path) for path in source_files.split(';') if os.path.splitext(path)[1] in HEADER_EXTENSIONS]

    cache = caching.load_cache(build_dir)
    source_files = caching.filter_processed_files(cache, source_files)
    source_files = caching.discard_files(source_files)

    print(f"Files to process: {len(source_files)}")
    print("Discarding took {} seconds".format(time.time() - start))
    
    data = code_extraction.process_files(include_dirs, source_files)
    yaml_data = yaml.dump(data, sort_keys=False)
    with open(os.path.join(build_dir, "reflection_data.yaml"), "w") as f:
        f.write(yaml_data)
    with open(os.path.join(build_dir, "reflection_data.json"), "w") as f:
        json.dump(data, f, sort_keys=False, indent=4)
    with open(os.path.join(build_dir, "serialization.cpp"), "w") as f:
        cpp = code_generation.generate_cpp_serialization_code(data)
        cpp += code_generation.generate_imgui_code(data)
        f.write(cpp)
    code_generation.generate_yaml_files(data, build_dir)
    caching.save_cache(build_dir, cache)
    print("Total time: {} seconds".format(time.time() - start))
if __name__ == "__main__":
    main()
