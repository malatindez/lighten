
cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dle_internship LANGUAGES CXX C)

set(ENGINE_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/engine")
file(GLOB_RECURSE ENGINE_SOURCES ${SRC_DIR} *.*)
list(FILTER ENGINE_SOURCES INCLUDE REGEX ${ENGINE_SOURCES_DIR}/*)

foreach(_source IN ITEMS ${ENGINE_SOURCES})
if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
else()
    set(_source_rel "${_source}")
endif()
get_filename_component(_source_path "${_source_rel}" PATH)
string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(engine ${ENGINE_SOURCES})
target_include_directories(engine PRIVATE ${ENGINE_SOURCES_DIR})


set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)