
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

set(PROJECT_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/project")
file(GLOB_RECURSE PROJECT_SOURCES ${SRC_DIR} *.*)
list(FILTER PROJECT_SOURCES INCLUDE REGEX ${PROJECT_SOURCES_DIR}/*)

foreach(_source IN ITEMS ${PROJECT_SOURCES})
if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
else()
    set(_source_rel "${_source}")
endif()
get_filename_component(_source_path "${_source_rel}" PATH)
string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Rpass=inline -Winline -W4 -mavx")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Winline /W4 /xCOMMON-AVX512")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4 /arch:AVX512")
endif()

add_subdirectory(third_party/entt)

add_executable(project WIN32 ${PROJECT_SOURCES} ${ENGINE_SOURCES})
target_include_directories(project PRIVATE ${PROJECT_SOURCES_DIR})
target_include_directories(project PRIVATE ${ENGINE_SOURCES_DIR})
target_link_libraries(project PUBLIC EnTT::EnTT)

set_target_properties(project PROPERTIES LINKER_LANGUAGE CXX)
file(GLOB RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${RESOURCES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
file(COPY ${RESOURCES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release")


set(TEST_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
file(GLOB_RECURSE TEST_SOURCES ${SRC_DIR} *.*)
list(FILTER TEST_SOURCES INCLUDE REGEX ${TEST_SOURCES_DIR}/*)

add_executable(tests ${TEST_SOURCES} ${ENGINE_SOURCES})
target_include_directories(tests PRIVATE ${TEST_SOURCES})
target_include_directories(tests PRIVATE ${ENGINE_SOURCES_DIR})
target_link_libraries(tests PUBLIC EnTT::EnTT)

set_target_properties(tests PROPERTIES LINKER_LANGUAGE CXX)