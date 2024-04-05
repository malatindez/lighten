import os
import json
BUILD_DIR = "../../build/"
TARGET_DATA_CACHE_FOLDER = os.path.join(BUILD_DIR, "target_data_cache/")

def get_target_data_cache(target):
    target_data_cache = os.path.join(TARGET_DATA_CACHE_FOLDER, target)
    # load json from this path
    with open(target_data_cache, 'r') as f:
        return json.load(f)
sources = set()
includes = set()

def load_sources(cache):
    sources.add(cache['source_dir'])

    for include in cache['include_directories']:
        if include.startswith("<"):
            includes.add(include[include.find(":")+1:include.find(">")])
        else:
            includes.add(include)

load_sources(get_target_data_cache("lighten.json"))
load_sources(get_target_data_cache("editor.json"))

# should be added by default from CMake in the future
# includes.add(r"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.38.33130\include")

cmd_string = "perl"
cmd_string += " ../../third_party/tools/C-include-2-dot/cinclude2dot"
cmd_string += " --merge=module"
cmd_string += " --src=" + ",".join(f'"{s}"' for s in sources)
cmd_string += " --include=" + ",".join(f'"{s}"' for s in includes)
cmd_string += " --quotetype=both > dependency.dot"

print(cmd_string)
try:
    os.system(cmd_string)
except:
    pass

try:
    print("Making svg")
    if not os.path.exists("docs"):
        os.mkdir("docs")
    os.system("dot -q5 -s4096 dependency.dot -Tsvg -o docs/source_code_graph.svg")
except Exception as e:
    print(e)
    pass