function(update_sources_msvc SOURCE_DIR INPUT_SOURCES)
  foreach(_source IN ITEMS ${INPUT_SOURCES})
    if(IS_ABSOLUTE "${_source}")
      file(RELATIVE_PATH _source_rel "${SOURCE_DIR}" "${_source}")
    else()
      set(_source_rel "${_source}")
      set(_source_rel "${_source}")
    endif()

    get_filename_component(_source_path "${_source_rel}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
  endforeach()
endfunction(update_sources_msvc)

function(update_runtime_directory_msvc PROJECT_NAME RUNTIME_DIRECTORY)
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user" "\
<?xml version=\"1.0\" encoding=\"utf-8\"?> \ 
<Project ToolsVersion=\"Current\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>${RUNTIME_DIRECTORY}/</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>${RUNTIME_DIRECTORY}/</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>${RUNTIME_DIRECTORY}/</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>${RUNTIME_DIRECTORY}/</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x32'\"> \ 
    <LocalDebuggerWorkingDirectory>${RUNTIME_DIRECTORY}/</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|x32'\"> \ 
    <LocalDebuggerWorkingDirectory>${RUNTIME_DIRECTORY}/</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x32'\"> \ 
    <LocalDebuggerWorkingDirectory>${RUNTIME_DIRECTORY}/</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|x32'\"> \ 
    <LocalDebuggerWorkingDirectory>${RUNTIME_DIRECTORY}/</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
</Project> \ 
")
endfunction(update_runtime_directory_msvc)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(MAXIMUM_WARNING_FLAG "-Wall -Wextra -Wpedantic" CACHE STRING "Most aggressive warning flag for Clang and GCC")
  set(INLINING_FLAG "-Rpass=inline" CACHE STRING "Inlining flag for Clang and GCC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(MAXIMUM_WARNING_FLAG "-Wall -Wextra -Wpedantic" CACHE STRING "Most aggressive warning flag for Clang and GCC")
  set(INLINING_FLAG "-finline-functions" CACHE STRING "Inlining flag for Clang and GCC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(INLINING_FLAG "" CACHE STRING "Inlining flag for Intel")
  set(MAXIMUM_WARNING_FLAG "/W4" CACHE STRING "Most aggressive warning flag for Intel")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(INLINING_FLAG "" CACHE STRING "Inlining flag for MSVC")
  set(MAXIMUM_WARNING_FLAG "/W4" CACHE STRING "Most aggressive warning flag for MSVC")
endif()

make_directory(${CMAKE_CURRENT_LIST_DIR}/../binary)
set(BINARY_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/../binary)