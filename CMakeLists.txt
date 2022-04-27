
cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dle_internship LANGUAGES CXX C)

set(LIGHTEN_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE LIGHTEN_SOURCES ${SRC_DIR} *.*)
list(FILTER LIGHTEN_SOURCES INCLUDE REGEX ${LIGHTEN_SOURCES_DIR}/*)

foreach(_source IN ITEMS ${LIGHTEN_SOURCES})
if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
else()
    set(_source_rel "${_source}")
endif()
get_filename_component(_source_path "${_source_rel}" PATH)
string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(dle_internship ${LIGHTEN_SOURCES})
target_include_directories(dle_internship PRIVATE ${LIGHTEN_SOURCES_DIR})


set_target_properties(dle_internship PROPERTIES LINKER_LANGUAGE CXX)