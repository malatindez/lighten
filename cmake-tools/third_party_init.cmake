set(THIRD_PARTY_INITIALIZED TRUE)

set(LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party")

set(GLM_DIR "${LIB_DIR}/glm")
set(MATH_DIR "${LIB_DIR}/math")
set(ASSIMP_DIR "${LIB_DIR}/assimp")
set(IMGUI_DIR "${LIB_DIR}/imgui")
set(IMGUIZMO_DIR "${LIB_DIR}/imguizmo")
set(IMGUI_CONSOLE_DIR "${LIB_DIR}/imgui_console")
set(DIRECTXTEX_DIR "${LIB_DIR}/microsoft/DirectXTex")
set(DDS_TEXTURE_LOADER_DIR "${LIB_DIR}/microsoft/DDSTextureLoader")
set(WIC_TEXTURE_LOADER_DIR "${LIB_DIR}/microsoft/WICTextureLoader")
set(ENTT_DIR "${LIB_DIR}/entt")
set(GTEST_DIR "${LIB_DIR}/google/google_test")
set(BENCHMARK_DIR "${LIB_DIR}/google/google_benchmark")
set(SPDLOG_DIR "${LIB_DIR}/spdlog")
set(MAL_PACKET_WEAVER_DIR "${LIB_DIR}/mal-packet-weaver")
set(MAL_TOOLKIT_DIR "${MAL_PACKET_WEAVER_DIR}/third_party/mal-toolkit/")

find_package(Boost REQUIRED COMPONENTS system serialization context coroutine HINTS "
  C:/" 
  "C:/Boost" 
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost")

# directx
include("${CMAKE_CURRENT_LIST_DIR}/FindDirectX.cmake")

if(NOT DirectX11_FOUND)
  message("WARNING: DirectX11 not found. Trying fallback solution...")
  set(DirectX_D3D11_INCLUDE_DIRS "")
  set(DirectX_D3D11_LIBRARIES d3d11.lib dxguid.lib dxgi.lib d3dcompiler.lib)
endif()

if(NOT DirectX12_FOUND)
  message("WARNING: DirectX12 not found. Trying fallback solution...")
  set(DirectX_D3D12_INCLUDE_DIRS "")
  set(DirectX_D3D12_LIBRARIES d3d12.lib dxgi.lib d3dcompiler.lib)
endif()

set(GLM_INCLUDE_DIRS "${GLM_DIR}")
set(MATH_INCLUDE_DIRS "${MATH_DIR}/include")

set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND OFF)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
add_subdirectory("${ASSIMP_DIR}")

set_target_properties(assimp PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(assimp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

set(ASSIMP_INCLUDE_DIRS "${ASSIMP_DIR}")
set(ASSIMP_LIBRARIES assimp)

# imgui
add_library(imgui STATIC "${IMGUI_DIR}/imgui.cpp"
  "${IMGUI_DIR}/imgui_draw.cpp"
  "${IMGUI_DIR}/imgui_demo.cpp"
  "${IMGUI_DIR}/imgui_widgets.cpp"
  "${IMGUI_DIR}/imgui_tables.cpp")
target_include_directories(imgui PUBLIC "${IMGUI_DIR}")
target_compile_features(imgui PUBLIC cxx_std_17)

set(IMGUI_INCLUDE_DIRS "${IMGUI_DIR}")
set(IMGUI_LIBRARIES imgui)

#  target_sources("${TARGET}" PUBLIC "${IMGUI_DIR}/backends/imgui_impl_dx11.cpp"
#                 "${IMGUI_DIR}/backends/imgui_impl_win32.cpp")

# imguizmo
add_library(imguizmo STATIC "${IMGUIZMO_DIR}/GraphEditor.cpp"
  "${IMGUIZMO_DIR}/ImCurveEdit.cpp"
  "${IMGUIZMO_DIR}/ImGradient.cpp"
  "${IMGUIZMO_DIR}/ImGuizmo.cpp"
  "${IMGUIZMO_DIR}/ImSequencer.cpp")
target_include_directories(imguizmo PUBLIC "${IMGUI_DIR}")
target_include_directories(imguizmo PUBLIC "${IMGUIZMO_DIR}")
target_link_libraries(imguizmo PUBLIC imgui)

set(IMGUIZMO_INCLUDE_DIRS "${IMGUIZMO_DIR}")
set(IMGUIZMO_LIBRARIES imguizmo)

# entt
add_subdirectory(${ENTT_DIR})
set(ENTT_LIBRARIES EnTT::EnTT)

# google test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("${GTEST_DIR}" EXCLUDE_FROM_ALL)

set_target_properties(gtest PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(gtest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

set_target_properties(gtest_main PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(gtest_main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

set(GTEST_INCLUDE_DIRS "${GTEST_DIR}/include" "${gtest_SOURCE_DIR}")
set(GTEST_LIBRARIES gtest gtest_main)

add_subdirectory("${BENCHMARK_DIR}" EXCLUDE_FROM_ALL)

#  target_link_libraries("${TARGET}" benchmark)

# spdlog
add_subdirectory("${SPDLOG_DIR}" EXCLUDE_FROM_ALL)


# DDS texture loader
add_library(dds_texture_loader11 STATIC "${DDS_TEXTURE_LOADER_DIR}/DDSTextureLoader11.cpp")
target_include_directories(dds_texture_loader11 PUBLIC "${DDS_TEXTURE_LOADER_DIR}")
set_target_properties(dds_texture_loader11 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(dds_texture_loader11 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
target_compile_features(dds_texture_loader11 PRIVATE cxx_std_17)
target_include_directories(dds_texture_loader11 PRIVATE ${DirectX_D3D11_INCLUDE_DIRS})
target_link_libraries(dds_texture_loader11 PRIVATE ${DirectX_D3D11_LIBRARIES})

set(DDS_TEXTURE_LOADER_INCLUDE_DIRS "${DDS_TEXTURE_LOADER_DIR}")
set(DDS_TEXTURE_LOADER_LIBRARIES dds_texture_loader11)

# WIC texture loader
add_library(wic_texture_loader11 STATIC "${WIC_TEXTURE_LOADER_DIR}/WICTextureLoader11.cpp")
target_include_directories(wic_texture_loader11 PUBLIC "${WIC_TEXTURE_LOADER_DIR}")
set_target_properties(wic_texture_loader11 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(wic_texture_loader11 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
target_compile_features(wic_texture_loader11 PRIVATE cxx_std_17)
target_include_directories(wic_texture_loader11 PRIVATE ${DirectX_D3D11_INCLUDE_DIRS})
target_link_libraries(wic_texture_loader11 PRIVATE ${DirectX_D3D11_LIBRARIES})

set(WIC_TEXTURE_LOADER_INCLUDE_DIRS "${WIC_TEXTURE_LOADER_DIR}")
set(WIC_TEXTURE_LOADER_LIBRARIES wic_texture_loader11)

# DirectXTex
file(GLOB_RECURSE DIRECTXTEX_SOURCES ${DIRECTXTEX_DIR}/*.cpp ${DIRECTXTEX_DIR}/*.hpp ${DIRECTXTEX_DIR}/*.h)
add_library(DirectXTex STATIC "${DIRECTXTEX_SOURCES}")
target_include_directories(DirectXTex PUBLIC "${DIRECTXTEX_DIR}/include" "${DIRECTXTEX_DIR}/src" "${DIRECTXTEX_DIR}/src/Shaders/Compiled")
set_target_properties(DirectXTex PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(DirectXTex PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
target_compile_features(DirectXTex PRIVATE cxx_std_17)
target_include_directories(DirectXTex PRIVATE ${DirectX_D3D11_INCLUDE_DIRS})
target_link_libraries(DirectXTex PRIVATE ${DirectX_D3D11_LIBRARIES})

set(DIRECTXTEX_INCLUDE_DIRS "${DIRECTXTEX_DIR}/include")
set(DIRECTXTEX_LIBRARIES DirectXTex)

# mal-packet-weaver
add_subdirectory(${MAL_PACKET_WEAVER_DIR})

set(MAL_PACKET_WEAVER_INCLUDE_DIRS "${MAL_PACKET_WEAVER_DIR}/mal-packet-weaver")
set(MAL_PACKET_WEAVER_LIBRARIES mal-packet-weaver)

# mal-toolkit
set(MAL_TOOLKIT_INCLUDE_DIRS "${MAL_TOOLKIT_DIR}/mal-toolkit")
set(MAL_TOOLKIT_LIBRARIES mal-toolkit)


message(STATUS "Third party libraries initialized")
