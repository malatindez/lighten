cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(server LANGUAGES CXX)
cmake_policy(SET CMP0060 NEW)

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake-tools/common.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake-tools/third_party.cmake")

set(SERVER_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE SERVER_SOURCES ${SERVER_SOURCES_DIR}/*.*)

update_sources_msvc("${CMAKE_CURRENT_SOURCE_DIR}" "${SERVER_SOURCES}")

add_executable(server ${SERVER_SOURCES})
target_link_libraries(server engine)

target_include_directories(server PRIVATE ${SERVER_SOURCES_DIR})

set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})