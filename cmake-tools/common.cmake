

make_directory(${CMAKE_CURRENT_LIST_DIR}/../binary)
set(BINARY_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/../binary)


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

function(aggregate_include_directories TARGET)
    # Initialize an empty list for aggregated includes
    set(AGGREGATED_INCLUDES "")

    # Check if the target is an ALIAS
    get_target_property(IS_ALIAS ${TARGET} ALIASED_TARGET)
    if(IS_ALIAS)
        set(TARGET ${IS_ALIAS}) # Use the original target
    endif()

    # Retrieve and add current include directories if they exist
    get_target_property(CURRENT_INCLUDES ${TARGET} INCLUDE_DIRECTORIES)
    if(NOT CURRENT_INCLUDES STREQUAL "CURRENT_INCLUDES-NOTFOUND")
        list(APPEND AGGREGATED_INCLUDES ${CURRENT_INCLUDES})
    endif()

    # Retrieve and add interface include directories if they exist
    get_target_property(CURRENT_INTERFACE_INCLUDES ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
    if(NOT CURRENT_INTERFACE_INCLUDES STREQUAL "CURRENT_INTERFACE_INCLUDES-NOTFOUND")
        list(APPEND AGGREGATED_INCLUDES ${CURRENT_INTERFACE_INCLUDES})
    endif()

    # Get linked libraries and process them
    get_target_property(LINKED_LIBS ${TARGET} INTERFACE_LINK_LIBRARIES)
    foreach(LIB in ${LINKED_LIBS})
        # Only process valid CMake targets
        if(TARGET ${LIB})
            # Recursively call the function for each linked library
            aggregate_include_directories(${LIB})
            get_target_property(LIB_INCLUDES ${LIB} REFLECTION_${LIB}_INCLUDE_DIRECTORIES)
            if(NOT LIB_INCLUDES STREQUAL "LIB_INCLUDES-NOTFOUND")
                list(APPEND AGGREGATED_INCLUDES ${LIB_INCLUDES})
            endif()
        endif()
    endforeach()

    # Remove duplicates
    list(REMOVE_DUPLICATES AGGREGATED_INCLUDES)

    # Set the property for the current target if it's not an alias
    if(NOT IS_ALIAS)
        set_target_properties(${TARGET} PROPERTIES REFLECTION_${TARGET}_INCLUDE_DIRECTORIES "${AGGREGATED_INCLUDES}")
    endif()
endfunction()


function(run_reflection_tool TARGET)
    # Aggregate include directories
    aggregate_include_directories(${TARGET})

    # Retrieve the aggregated include directories
    get_target_property(INPUT_TARGET_INCLUDES ${TARGET} REFLECTION_${TARGET}_INCLUDE_DIRECTORIES)
    get_target_property(INPUT_TARGET_SOURCES ${TARGET} SOURCES)
    get_target_property(INPUT_TARGET_BINARY_DIR ${TARGET} BINARY_DIR)

    # Run custom command with the aggregated include directories
    add_custom_command(
        TARGET "${TARGET}" PRE_BUILD
        COMMAND python "${CMAKE_CURRENT_LIST_DIR}/../reflection/main.py"
                        --build_dir "${INPUT_TARGET_BINARY_DIR}"
                        --include_dirs "${INPUT_TARGET_INCLUDES}"
                        --source_files "${INPUT_TARGET_SOURCES}"
        COMMENT "Running reflection tool for ${TARGET}"
    )
endfunction(run_reflection_tool)
