import argparse
import os.path
import argparse
import os
import re
import caching

pattern_dataclass = re.compile(r'\s*LIGHTEN_DATACLASS(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_component = re.compile(r'\s*LIGHTEN_COMPONENT(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_system = re.compile(r'\s*LIGHTEN_SYSTEM(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_method = re.compile(r'\s*LIGHTEN_METHOD(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')
pattern_property = re.compile(r'\s*LIGHTEN_PROPERTY\((.*?)\)\s*\n\s*(\w+)\s+(\w+);')
pattern_getter = re.compile(r'\s*LIGHTEN_PROPERTY_GETTER(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')
pattern_setter = re.compile(r'\s*LIGHTEN_PROPERTY_SETTER(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')

REFLECTION_FOLDER = os.path.dirname(os.path.abspath(__file__))
HEADER_EXTENSIONS = [".h", ".hpp", ".inl", ".hxx", ".hh", ".inc", ".ipp", ".ixx", ".i++", ".h++"]

def process_files(cache, build_dir, include_dirs, file_paths):
    for i in file_paths:
        print("Processing file: " + i)

def filter_processed_files(cache, file_paths):
    output = []
    if "files" not in cache:
        cache["files"] = {}
    for file in file_paths:
        if file not in cache["files"]:
            cache["files"][file] = {}
            cache["files"][file]["dataclasses"] = []
            cache["files"][file]["components"] = []
            cache["files"][file]["systems"] = []
            cache["files"][file]["last_time_modified"] = os.path.getmtime(file)
            cache["files"][file]["hash"] = caching.md5_speedcheck(file, 0x10000)
            output.append(file)
            continue
        file_cache = cache["files"][file]
        if ["last_time_modified"] == os.path.getmtime(file):
            continue
        hash = caching.md5_speedcheck(file, 0x10000)
        if file_cache["hash"] != hash:
            file_cache["dataclasses"] = []
            file_cache["components"] = []
            file_cache["systems"] = []
            file_cache["last_time_modified"] = os.path.getmtime(file)
            file_cache["hash"] = caching.md5_speedcheck(file, 0x10000)
            output.append(file)
    return output
def discard_files(file_paths):
    output = []
    for file in file_paths:
        with open(file, "r") as f:
            content = f.read()
            if (not pattern_dataclass.search(content) and 
                not pattern_component.search(content) and 
                not pattern_system.search(content)):
                continue
        output.append(file)
    return output

def main():
    parser = argparse.ArgumentParser(description='Process some include directories.')
    parser.add_argument('--build_dir', help='Build directory')
    parser.add_argument('--include_dirs', help='Include directories (semicolon-separated)')
    parser.add_argument('--source_files', help='Source files (semicolon-separated)')
    args = parser.parse_args()

    include_dirs = args.include_dirs.split(';') if args.include_dirs else []
    source_files = args.source_files.split(';') if args.source_files else []
    source_files = [os.path.normpath(path) for path in source_files if os.path.splitext(path)[1] in HEADER_EXTENSIONS]

    build_dir = os.path.normpath(args.build_dir)

    cache = caching.load_cache(build_dir)

    source_files  = filter_processed_files(cache, source_files)

    source_files = discard_files(source_files)
    
    process_files(cache, build_dir, include_dirs, source_files)

    caching.save_cache(build_dir, cache)

if __name__ == "__main__":
    main()
