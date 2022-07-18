set(D3D_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/d3d")
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

target_add_libraries(d3d_test "entt" "spdlog")

include("third_party/FindDirectX.cmake")
link_directories(${DirectX_LIBRARY_DIR})
target_link_libraries(d3d_test ${DirectX_LIBRARY_DIR}/d3d11.lib)
target_link_libraries(d3d_test ${DirectX_LIBRARY_DIR}/d3d10.lib)
target_link_libraries(d3d_test ${DirectX_LIBRARY_DIR}/dxguid.lib)
target_link_libraries(d3d_test ${DirectX_LIBRARY_DIR}/dxgi.lib)
target_link_libraries(d3d_test ${DirectX_LIBRARY_DIR}/d3dcompiler.lib)



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