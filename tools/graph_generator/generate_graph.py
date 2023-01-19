import os
source = [
    "engine", 
    "d3d_test"
]
include = [
    "engine", 
    "d3d_test",
    "third_party/microsoft/DDSTextureLoader",
    "third_party/spdlog/include",
    "third_party/google/google_test/googletest/include",
    "third_party/google/google_test/googletest",
    "third_party/assimp/include",
    "build/third_party/assimp/include",
    "third_party/imgui",
    "third_party/imguizmo",
    "third_party/imgui_console/include",
    "third_party/imgui_console/include/imgui_console",
    "third_party/entt/src",
    "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.33.31629/include",
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/um",
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/ucrt"
]

cmd_string = "perl"
cmd_string += " third_party/tools/C-include-2-dot/cinclude2dot"
cmd_string += " --merge=module"
cmd_string += " --src=" + ",".join(f'"{s}"' for s in source)
cmd_string += " --include=" + ",".join(f'"{s}"' for s in include)
cmd_string += " --quotetype=both > dependency.dot"

os.system(cmd_string)

os.system("dot -q5 -s4096 dependency.dot -Tsvg -o docs/source_code_graph.svg")