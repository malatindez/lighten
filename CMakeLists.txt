cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(engine LANGUAGES CXX)
cmake_policy(SET CMP0060 NEW)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake-tools/common.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake-tools/third_party.cmake")

include("${CMAKE_CURRENT_SOURCE_DIR}/engine/CMakeLists.txt")
include("${CMAKE_CURRENT_SOURCE_DIR}/editor/CMakeLists.txt")
include("${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
include("${CMAKE_CURRENT_SOURCE_DIR}/server/CMakeLists.txt")
