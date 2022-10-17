option(test "build all tests." ON)

if(test)
  set(TEST_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
  file(GLOB_RECURSE TEST_SOURCES ${TEST_SOURCES_DIR}/*.*)

  foreach(_source IN ITEMS ${TEST_SOURCES})
    if(IS_ABSOLUTE "${_source}")
      file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
    else()
      set(_source_rel "${_source}")
    endif()

    get_filename_component(_source_path "${_source_rel}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
  endforeach()

  add_executable(runUnitTests ${TEST_SOURCES} ${ENGINE_SOURCES})

  include("third_party/FindDirectX.cmake")
  link_directories(${DirectX_LIBRARY_DIR})
  target_link_libraries(runUnitTests ${DirectX_LIBRARY_DIR}/d3d11.lib)
  target_link_libraries(runUnitTests ${DirectX_LIBRARY_DIR}/d3d10.lib)
  target_link_libraries(runUnitTests ${DirectX_LIBRARY_DIR}/dxguid.lib)
  target_link_libraries(runUnitTests ${DirectX_LIBRARY_DIR}/dxgi.lib)
  target_link_libraries(runUnitTests ${DirectX_LIBRARY_DIR}/d3dcompiler.lib)
  target_include_directories(runUnitTests PRIVATE ${TEST_SOURCES_DIR})
  target_include_directories(runUnitTests PRIVATE ${ENGINE_SOURCES_DIR})

  target_compile_options(runUnitTests PRIVATE /bigobj)
  target_add_libraries(runUnitTests "benchmark" "math" "gtest" "entt" "spdlog" "directx" "assimp" "glm" "imgui" "imguizmo" "imgui_console" "directxtex" "dds_texture_loader11" "wic_texture_loader11" "glm")

  add_test(NAME TEST COMMAND runUnitTests)

  set_target_properties(runUnitTests PROPERTIES LINKER_LANGUAGE CXX)
  file(WRITE "${CMAKE_BINARY_DIR}/runUnitTests.vcxproj.user" "\
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
  set_target_properties(runUnitTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
endif()
