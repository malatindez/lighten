@echo off
perl ../third_party/tools/C-include-2-dot/cinclude2dot --merge=module --src=../engine/,../d3d_test --include=../engine/,../d3d_test,../third_party/microsoft/DDSTextureLoader,../third_party/spdlog/include,../third_party/google/google_test/googletest/include,../third_party/google/google_test/googletest,../third_party/assimp/code/../include,../build/third_party/assimp/code/../include,../third_party/imgui,../third_party/imguizmo,../third_party/imgui_console/include,../third_party/imgui_console/include/imgui_console,../third_party/entt/src,"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.32.31326\include","C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\um","C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\ucrt" --quotetype=both > dependency.dot
dot -q5 -s4096 dependency.dot -Tsvg -o out.svg 