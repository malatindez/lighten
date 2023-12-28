set(ENGINE_SOURCES_DIR "${CMAKE_CURRENT_LIST_DIR}")
file(GLOB_RECURSE ENGINE_SOURCES ${ENGINE_SOURCES_DIR}/*.*)
update_sources_msvc("${CMAKE_CURRENT_LIST_DIR}" "${ENGINE_SOURCES}")

add_library(engine STATIC "${ENGINE_SOURCES}")
target_include_directories(engine PUBLIC ${MATH_INCLUDE_DIRS})
target_include_directories(engine PUBLIC ${ENGINE_SOURCES_DIR})
target_link_libraries(engine PUBLIC EnTT::EnTT spdlog::spdlog_header_only)

target_include_directories(engine PUBLIC 
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
)
target_link_libraries(engine PUBLIC 
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
)

target_sources(engine PUBLIC "${IMGUI_DIR}/backends/imgui_impl_dx11.cpp" "${IMGUI_DIR}/backends/imgui_impl_win32.cpp")

set_target_properties(engine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(engine PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

set_target_properties(engine PROPERTIES UNITY_BUILD TRUE)
set_target_properties(engine PROPERTIES UNITY_BUILD_MODE BATCH)
set_target_properties(engine PROPERTIES UNITY_BUILD_BATCH_SIZE 4)

target_compile_definitions(engine PUBLIC _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)

target_compile_options(engine PRIVATE ${INLINING_FLAG} ${MAXIMUM_WARNING_FLAG} /MP)
update_runtime_directory_msvc(engine ${CMAKE_CURRENT_LIST_DIR}/../runtime_directory)

