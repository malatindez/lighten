cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tests LANGUAGES CXX)
cmake_policy(SET CMP0060 NEW)

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake-tools/common.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake-tools/third_party.cmake")

option(test "build all tests." OFF)

if(test)
  set(TEST_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
  file(GLOB_RECURSE TEST_SOURCES ${TEST_SOURCES_DIR}/*.*)
  update_sources_msvc("${CMAKE_CURRENT_SOURCE_DIR}" "${TEST_SOURCES}")

  add_executable(runUnitTests ${TEST_SOURCES} ${ENGINE_SOURCES})

  target_include_directories(runUnitTests PRIVATE ${TEST_SOURCES_DIR})
  target_include_directories(runUnitTests PUBLIC ${GTEST_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
  target_link_libraries(runUnitTests lighten ${GTEST_LIBRARIES})

  target_compile_options(runUnitTests PRIVATE /bigobj)

  add_test(NAME TEST COMMAND runUnitTests)
  set_target_properties(runUnitTests PROPERTIES LINKER_LANGUAGE CXX)
  set_target_properties(runUnitTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
  target_compile_options(runUnitTests PRIVATE "${INLINING_FLAG}" "${MAXIMUM_WARNING_FLAG}" "/MP")
  update_runtime_directory_msvc(runUnitTests ${CMAKE_CURRENT_SOURCE_DIR}/../runtime_directory)
endif()
