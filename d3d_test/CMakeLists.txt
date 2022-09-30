set(D3D_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/d3d_test")

file(GLOB_RECURSE D3D_SOURCES ${D3D_SOURCES_DIR} *.*)
list(FILTER D3D_SOURCES INCLUDE REGEX ${D3D_SOURCES_DIR}/*)

foreach(_source IN ITEMS ${D3D_SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()

  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(d3d_test WIN32 ${D3D_SOURCES} ${ENGINE_SOURCES})
target_include_directories(d3d_test PRIVATE ${D3D_SOURCES_DIR})
target_include_directories(d3d_test PRIVATE ${ENGINE_SOURCES_DIR})

# target_link_libraries(d3d_test engine)
target_add_libraries(d3d_test "entt" "spdlog" "directx" "assimp" "imgui" "imguizmo" "imgui_console" "directxtex" "dds_texture_loader11" "wic_texture_loader11")

file(GLOB RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${RESOURCES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
file(COPY ${RESOURCES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release")

set_target_properties(d3d_test PROPERTIES LINKER_LANGUAGE CXX)
file(WRITE "${CMAKE_BINARY_DIR}/d3d_test.vcxproj.user" "\
<?xml version=\"1.0\" encoding=\"utf-8\"?> \ 
<Project ToolsVersion=\"Current\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>$(ProjectDir)..\\runtime_directory\\</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>$(ProjectDir)..\\runtime_directory\\</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>$(ProjectDir)..\\runtime_directory\\</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>$(ProjectDir)..\\runtime_directory\\</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
</Project> \ 
")

set_target_properties(d3d_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})