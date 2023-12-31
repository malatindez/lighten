import json
import os
import hashlib
import re

pattern_dataclass = re.compile(r'\s*LIGHTEN_DATACLASS(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_component = re.compile(r'\s*LIGHTEN_COMPONENT(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_system = re.compile(r'\s*LIGHTEN_SYSTEM(?:\((.*?)\))?\s*(?:\n\s*)?(class|struct)\s+(\w+)')
pattern_method = re.compile(r'\s*LIGHTEN_METHOD(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')
pattern_property = re.compile(r'\s*LIGHTEN_PROPERTY\((.*?)\)\s*\n\s*(\w+)\s+(\w+);')
pattern_getter = re.compile(r'\s*LIGHTEN_PROPERTY_GETTER(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')
pattern_setter = re.compile(r'\s*LIGHTEN_PROPERTY_SETTER(?:\((.*?)\))?\s*\n?\s*(\w+)\s+(\w+)\((.*?)\)\s*((?:[_a-zA-Z0-9]+\s*)*)?\s*(?:{|;)')

def md5_speedcheck(path, size):
    m = hashlib.md5()
    with open(path, 'rb') as f:
        b = f.read(size)
        while len(b) > 0:
            m.update(b)
            b = f.read(size)
    return m.hexdigest()

def load_cache(build_dir):
    cache_path = os.path.join(build_dir, "file_parse_cache.json")
    try:
        if os.path.exists(cache_path):
            with open(cache_path, "r") as f:
                return json.load(f)
    except:
        pass
    return {}

def save_cache(build_dir, cache):
    cache_path = os.path.join(build_dir, "file_parse_cache.json")
    with open(cache_path, "w") as f:
        json.dump(cache, f, indent=4, sort_keys=True)


def filter_processed_files(cache, file_paths):
    output = []
    if "files" not in cache:
        cache["files"] = {}
    for file in file_paths:
        if not os.path.exists(file):
            continue
        last_time_modified = os.path.getmtime(file)
        if file not in cache["files"]:
            cache["files"][file] = {}
            cache["files"][file]["last_time_modified"] = last_time_modified
            cache["files"][file]["hash"] = md5_speedcheck(file, 0x10000)
            output.append(file)
            continue
        file_cache = cache["files"][file]
        if file_cache["last_time_modified"] == last_time_modified:
            continue
        hash = md5_speedcheck(file, 0x10000)
        if file_cache["hash"] != hash:
            file_cache["last_time_modified"] = os.path.getmtime(file)
            file_cache["hash"] = hash
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