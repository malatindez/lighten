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

add_library(engine STATIC "${ENGINE_SOURCES}")
target_add_libraries(engine "entt" "spdlog" "directx" "assimp" "imgui" "imguizmo" "imgui_console" "dds_texture_loader")
target_include_directories(engine PRIVATE ${ENGINE_SOURCES_DIR})