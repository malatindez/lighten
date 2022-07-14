set(PROJECT_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/project")
file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCES_DIR} *.*)
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

add_executable(project WIN32 ${PROJECT_SOURCES} ${ENGINE_SOURCES})
target_include_directories(project PRIVATE ${PROJECT_SOURCES_DIR})
target_include_directories(project PRIVATE ${ENGINE_SOURCES_DIR})

target_add_libraries(project "entt" "spdlog")

file(GLOB RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${RESOURCES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
file(COPY ${RESOURCES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release")

set_target_properties(project PROPERTIES LINKER_LANGUAGE CXX)