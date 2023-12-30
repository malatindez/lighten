import argparse
import os.path
import argparse
import os
import re

pattern_dataclass = re.compile(r'\s*LIGHTEN_DATACLASS(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_component = re.compile(r'\s*LIGHTEN_COMPONENT(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_system = re.compile(r'\s*LIGHTEN_SYSTEM(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_method = re.compile(r'\s*LIGHTEN_METHOD(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')
pattern_property = re.compile(r'\s*LIGHTEN_PROPERTY\((.*?)\)\s*\n\s*(\w+)\s+(\w+);')
pattern_getter = re.compile(r'\s*LIGHTEN_PROPERTY_GETTER(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')
pattern_setter = re.compile(r'\s*LIGHTEN_PROPERTY_SETTER(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')
REFLECTION_FOLDER = os.path.dirname(os.path.abspath(__file__))
HEADER_EXTENSIONS = [".h", ".hpp", ".inl", ".hxx", ".hh", ".inc", ".ipp", ".ixx", ".i++", ".h++"]

def process_ast(translation_unit):
    pass

def process_files(include_dirs, file_paths):
    pass

def main():
    parser = argparse.ArgumentParser(description='Process some include directories.')
    parser.add_argument('--build_dir', help='Build directory')
    parser.add_argument('--include_dirs', help='Include directories (semicolon-separated)')
    parser.add_argument('--source_files', help='Source files (semicolon-separated)')
    args = parser.parse_args()

    include_dirs = args.include_dirs.split(';') if args.include_dirs else []
    source_files = args.source_files.split(';') if args.source_files else []
    source_files = [os.path.normpath(path) for path in source_files if os.path.splitext(path)[1] in HEADER_EXTENSIONS]
    process_files(include_dirs, source_files)

if __name__ == "__main__":
    main()