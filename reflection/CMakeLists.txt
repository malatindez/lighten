cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(reflection_tool LANGUAGES CXX)
cmake_policy(SET CMP0060 NEW)

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake-tools/common.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake-tools/third_party.cmake")

set(REFLECTION_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE REFLECTION_SOURCES ${REFLECTION_SOURCES_DIR}/*.*)

update_sources_msvc("${CMAKE_CURRENT_SOURCE_DIR}" "${REFLECTION_SOURCES}")

add_executable(reflection ${REFLECTION_SOURCES})
target_link_libraries(reflection engine)

target_include_directories(reflection PRIVATE ${REFLECTION_SOURCES_DIR})

set_target_properties(reflection PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})