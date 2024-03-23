cmake_minimum_required(VERSION 3.21)

project(engine LANGUAGES CXX)
cmake_policy(SET CMP0060 NEW)

# this should be always defined.
# it is used to determine the root directory of the project where should we build the engine and editor.
# Symlinks to the yaml files are created in the runtime directory so the engine can find them.
set(GLOBAL_RUNTIME_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/runtime_directory)

set(GLOBAL_REFLECTION_OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/engine/reflection/reflection.hpp)
set(GLOBAL_REFLECTION_CACHE_DIR ${CMAKE_CURRENT_BINARY_DIR}/reflection_cache)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake-tools/common.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake-tools/third_party.cmake")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/engine")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/editor")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/server")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/benchmarks")
