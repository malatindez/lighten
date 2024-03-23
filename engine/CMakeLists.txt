cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(lighten LANGUAGES CXX)
cmake_policy(SET CMP0060 NEW)

set(ENGINE_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE ENGINE_SOURCES ${ENGINE_SOURCES_DIR}/*.*)
update_sources_msvc("${CMAKE_CURRENT_SOURCE_DIR}" "${ENGINE_SOURCES}")

add_library(lighten STATIC "${ENGINE_SOURCES}")
target_include_directories(lighten PUBLIC ${ENGINE_SOURCES_DIR})

target_include_directories(lighten PUBLIC 
    ${DirectX_D3D11_INCLUDE_DIRS}
    ${DirectX_D3D12_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${IMGUI_INCLUDE_DIRS}
    ${IMGUIZMO_INCLUDE_DIRS}
    ${DDS_TEXTURE_LOADER_INCLUDE_DIRS}
    ${WIC_TEXTURE_LOADER_INCLUDE_DIRS}
    ${DIRECTXTEX_INCLUDE_DIRS}
    ${MAL_PACKET_WEAVER_INCLUDE_DIRS}
    ${MAL_TOOLKIT_INCLUDE_DIRS}
    ${JSON_INCLUDE_DIRS}
    ${YAML_CPP_INCLUDE_DIRS}
    ${CEREAL_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${ENTT_INCLUDE_DIRS}
    ${SPDLOG_INCLUDE_DIRS}
)
target_link_libraries(lighten PUBLIC 
    ${DirectX_D3D11_LIBRARIES}
    ${DirectX_D3D12_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    ${IMGUI_LIBRARIES}
    ${IMGUIZMO_LIBRARIES}
    ${DDS_TEXTURE_LOADER_LIBRARIES}
    ${WIC_TEXTURE_LOADER_LIBRARIES}
    ${DIRECTXTEX_LIBRARIES}
    ${MAL_PACKET_WEAVER_LIBRARIES}
    ${MAL_TOOLKIT_LIBRARIES}
    ${YAML_CPP_LIBRARIES}
    ${CEREAL_LIBRARIES}
)

target_sources(lighten PUBLIC "${IMGUI_DIR}/backends/imgui_impl_dx11.cpp" "${IMGUI_DIR}/backends/imgui_impl_win32.cpp")

set_target_properties(lighten PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(lighten PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

set_target_properties(lighten PROPERTIES UNITY_BUILD TRUE)
set_target_properties(lighten PROPERTIES UNITY_BUILD_MODE BATCH)
set_target_properties(lighten PROPERTIES UNITY_BUILD_BATCH_SIZE 4)

target_compile_definitions(lighten PUBLIC _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)

target_compile_options(lighten PRIVATE ${INLINING_FLAG} ${MAXIMUM_WARNING_FLAG} /MP)
update_runtime_directory_msvc(lighten "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_directory")


get_filename_component(output_dir "${GLOBAL_REFLECTION_OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${output_dir}")
if(NOT EXISTS "${GLOBAL_REFLECTION_OUTPUT_FILE}")
    file(TOUCH "${GLOBAL_REFLECTION_OUTPUT_FILE}")
endif()
target_sources(lighten PRIVATE "${GLOBAL_REFLECTION_OUTPUT_FILE}")

run_reflection_tool(lighten)