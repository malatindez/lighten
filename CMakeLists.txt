cmake_minimum_required(VERSION 3.21)

project(engine LANGUAGES CXX)
cmake_policy(SET CMP0060 NEW)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake-tools/common.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake-tools/third_party.cmake")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/engine")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/editor")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/server")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/benchmarks")