cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(benchmark_tests LANGUAGES CXX)
cmake_policy(SET CMP0060 NEW)

set(CPP_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE CPP_SOURCES ${CPP_SOURCES_DIR} *.*)
list(FILTER CPP_SOURCES INCLUDE REGEX ${CPP_SOURCES_DIR}/*)
set(BENCHMARK_SOURCES ${CPP_SOURCES})


set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_directory/assets/shaders")
file(GLOB_RECURSE SHADER_SOURCES ${SHADERS_DIR} *.*)
list(FILTER SHADER_SOURCES INCLUDE REGEX ${SHADERS_DIR}/*)

list(APPEND BENCHMARK_SOURCES ${SHADER_SOURCES})

update_sources_msvc("${CMAKE_CURRENT_SOURCE_DIR}" "${BENCHMARK_SOURCES}")

add_executable(benchmark_tests ${BENCHMARK_SOURCES})
target_include_directories(benchmark_tests PRIVATE ${CPP_SOURCES_DIR})
target_link_libraries(benchmark_tests PUBLIC engine benchmark ${GLM_LIBRARIES})
target_include_directories(
    benchmark_tests PUBLIC
    ${GLM_INCLUDE_DIRS}
)



target_compile_options(benchmark_tests PRIVATE /bigobj)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" "Zl")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL /EDITANDCONTINUE")


file(GLOB RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../resources/*")
file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${RESOURCES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
file(COPY ${RESOURCES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release")

set_target_properties(benchmark_tests PROPERTIES UNITY_BUILD TRUE)
set_target_properties(benchmark_tests PROPERTIES UNITY_BUILD_MODE BATCH)
set_target_properties(benchmark_tests PROPERTIES UNITY_BUILD_BATCH_SIZE 10)

set_target_properties(benchmark_tests PROPERTIES LINKER_LANGUAGE CXX)

update_runtime_directory_msvc(benchmark_tests "${CMAKE_CURRENT_SOURCE_DIR}/..//runtime_directory")
target_compile_options(benchmark_tests PRIVATE "${INLINING_FLAG}" "${MAXIMUM_WARNING_FLAG}" "/MP")
set_target_properties(benchmark_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})


run_reflection_tool(benchmark_tests)