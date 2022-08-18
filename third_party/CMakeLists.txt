set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

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

# directx
include("third_party/FindDirectX.cmake")

function(target_add_direct3d TARGET)
  target_link_directories("${TARGET}" PRIVATE ${DirectX_LIBRARY_DIR})
  target_link_libraries("${TARGET}" ${DirectX_LIBRARY_DIR}/d3d11.lib)
  target_link_libraries("${TARGET}" ${DirectX_LIBRARY_DIR}/d3d10.lib)
  target_link_libraries("${TARGET}" ${DirectX_LIBRARY_DIR}/dxguid.lib)
  target_link_libraries("${TARGET}" ${DirectX_LIBRARY_DIR}/dxgi.lib)
  target_link_libraries("${TARGET}" ${DirectX_LIBRARY_DIR}/d3dcompiler.lib)
endfunction(target_add_direct3d)

# assimp
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
add_subdirectory("${ASSIMP_DIR}")

function(target_add_assimp TARGET)
  target_link_libraries("${TARGET}" assimp)
  target_include_directories(assimp PRIVATE "${ASSIMP_DIR}")
endfunction(target_add_assimp)

set_target_properties(assimp PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})
set_target_properties(assimp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})

# imgui
add_library(imgui STATIC "${IMGUI_DIR}/imgui.cpp"
  "${IMGUI_DIR}/imgui_draw.cpp"
  "${IMGUI_DIR}/imgui_demo.cpp"
  "${IMGUI_DIR}/imgui_widgets.cpp"
  "${IMGUI_DIR}/imgui_tables.cpp")
target_include_directories(imgui PUBLIC "${IMGUI_DIR}")
target_compile_features(imgui PUBLIC cxx_std_17)

function(target_add_imgui TARGET)
  target_link_libraries("${TARGET}" imgui)
  target_include_directories(imguizmo PRIVATE "${IMGUI_DIR}")
  target_sources("${TARGET}" PUBLIC "${IMGUI_DIR}/backends/imgui_impl_dx11.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_win32.cpp")
endfunction(target_add_imgui)

# csys
add_library(csys INTERFACE)
target_include_directories(csys INTERFACE "${IMGUI_CONSOLE_DIR}/include")
target_compile_features(csys INTERFACE cxx_std_17)

# imgui_console
add_library(imgui_console STATIC "${IMGUI_CONSOLE_DIR}/src/imgui_console.cpp" "${IMGUI_CONSOLE_DIR}/include/imgui_console/imgui_console.h")
target_include_directories(imgui_console PUBLIC "${IMGUI_DIR}")
target_include_directories(imgui_console PUBLIC "${IMGUI_CONSOLE_DIR}/include")
target_include_directories(imgui_console PUBLIC "${IMGUI_CONSOLE_DIR}/include/imgui_console")
target_compile_features(imgui_console PUBLIC cxx_std_17)
target_link_libraries(imgui_console PUBLIC csys)
target_link_libraries(imgui_console PUBLIC imgui)

function(target_add_imgui_console TARGET)
  target_link_libraries("${TARGET}" imgui_console)
  target_include_directories("${TARGET}" PRIVATE "${IMGUI_CONSOLE}/include")
  target_include_directories(imgui_console PRIVATE "${IMGUI_CONSOLE}/include/imgui_console")
endfunction(target_add_imgui_console)

# imguizmo
add_library(imguizmo STATIC "${IMGUIZMO_DIR}/GraphEditor.cpp"
  "${IMGUIZMO_DIR}/ImCurveEdit.cpp"
  "${IMGUIZMO_DIR}/ImGradient.cpp"
  "${IMGUIZMO_DIR}/ImGuizmo.cpp"
  "${IMGUIZMO_DIR}/ImSequencer.cpp")
target_include_directories(imguizmo PUBLIC "${IMGUI_DIR}")
target_include_directories(imguizmo PUBLIC "${IMGUIZMO_DIR}")
target_link_libraries(imguizmo PUBLIC imgui)

function(target_add_imguizmo TARGET)
  target_link_libraries("${TARGET}" imguizmo)
  target_include_directories(imguizmo PRIVATE "${IMGUIZMO_DIR}")
endfunction(target_add_imguizmo)

# entt
add_subdirectory(third_party/entt)

function(target_add_entt TARGET)
  target_link_libraries("${TARGET}" EnTT::EnTT)
endfunction(target_add_entt)

# google test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("${GTEST_DIR}" EXCLUDE_FROM_ALL)

function(target_add_gtest TARGET)
  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  include_directories(${INCLUDES})
  target_link_libraries("${TARGET}" gtest gtest_main)
endfunction(target_add_gtest)

# google benchmark
add_subdirectory("${BENCHMARK_DIR}" EXCLUDE_FROM_ALL)

function(target_add_benchmark TARGET)
  target_link_libraries("${TARGET}" benchmark)
endfunction(target_add_benchmark)

# spdlog
add_subdirectory("${SPDLOG_DIR}" EXCLUDE_FROM_ALL)

function(target_add_spdlog TARGET)
  target_include_directories("${TARGET}" PRIVATE "${SPDLOG_DIR}/include")
endfunction(target_add_spdlog)

# DDS texture loader
add_library(dds_texture_loader11 STATIC "${DDS_TEXTURE_LOADER_DIR}/DDSTextureLoader11.cpp")
target_include_directories(dds_texture_loader11 PUBLIC "${DDS_TEXTURE_LOADER_DIR}")
set_target_properties(dds_texture_loader11 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})
set_target_properties(dds_texture_loader11 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})
target_compile_features(dds_texture_loader11 PUBLIC cxx_std_17)
target_add_direct3d(dds_texture_loader11)


function(target_add_dds_texture_loader TARGET)
  target_include_directories("${TARGET}" PRIVATE "${DDS_TEXTURE_LOADER_DIR}")
  target_link_libraries("${TARGET}" dds_texture_loader11)
endfunction(target_add_dds_texture_loader)

# WIC texture loader
add_library(wic_texture_loader11 STATIC "${WIC_TEXTURE_LOADER_DIR}/WICTextureLoader11.cpp")
target_include_directories(wic_texture_loader11 PUBLIC "${WIC_TEXTURE_LOADER_DIR}")
set_target_properties(wic_texture_loader11 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})
set_target_properties(wic_texture_loader11 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})
target_compile_features(wic_texture_loader11 PUBLIC cxx_std_17)
target_add_direct3d(wic_texture_loader11)

function(target_add_wic_texture_loader TARGET)
  target_include_directories("${TARGET}" PRIVATE "${WIC_TEXTURE_LOADER_DIR}")
  target_link_libraries("${TARGET}" wic_texture_loader11)
endfunction(target_add_wic_texture_loader)

# DirectXTex
file(GLOB_RECURSE DIRECTXTEX_SOURCES ${DIRECTXTEX_DIR}/*.cpp ${DIRECTXTEX_DIR}/*.hpp ${DIRECTXTEX_DIR}/*.h)
add_library(DirectXTex STATIC "${DIRECTXTEX_SOURCES}")
target_include_directories(DirectXTex PUBLIC "${DIRECTXTEX_DIR}/include" "${DIRECTXTEX_DIR}/src" "${DIRECTXTEX_DIR}/src/Shaders/Compiled")
set_target_properties(DirectXTex PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})
set_target_properties(DirectXTex PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})
target_compile_features(DirectXTex PUBLIC cxx_std_17)
target_add_direct3d(DirectXTex)

function(target_add_directxtex TARGET)
  target_include_directories("${TARGET}" PRIVATE "${DIRECTXTEX_DIR}/include")
  target_link_libraries("${TARGET}" DirectXTex)
endfunction(target_add_directxtex)

function(target_add_libraries TARGET)
  if("${ARGN}" MATCHES "[dD][iI][rR][eE][cC][tT][xX]")
    target_add_direct3d(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[dD][iI][rR][eE][cC][tT]3[dD]")
    target_add_direct3d(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[aA][sS][sS][iI][mM][pP]")
    target_add_assimp(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[iI][mM][gG][uU][iI]")
    target_add_imgui(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[iI][mM][gG][uU][iI][zZ][mM][oO]")
    target_add_imguizmo(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[iI][mM][gG][uU][iI]_[cC][oO][nN][sS][oO][lL][eE]")
    target_add_imgui_console(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[eE][nN][tT][tT]")
    target_add_entt(${TARGET})
  endif()

  if("${ARGN}" MATCHES "gtest")
    target_add_gtest(${TARGET})
  endif()

  if("${ARGN}" MATCHES "([gG]([oO]{2}[gG][lL][eE]){0,1}){0,1}[_]{0,1}[bB][eE][nN][cC][hH][mM][aA][rR][kK]")
    target_add_benchmark(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[sS][pP][dD][lL][oO][gG]")
    target_add_spdlog(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[dD][iI][rR][eE][cC][tT][xX][tT][eE][xX]")
    target_add_directxtex(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[dD][dD][sS]_[tT][eE][xX][tT][uU][rR][eE]_[lL][oO][aA][dD][eE][rR]11")
    target_add_dds_texture_loader(${TARGET})
  endif()

  if("${ARGN}" MATCHES "[wW][iI][cC]_[tT][eE][xX][tT][uU][rR][eE]_[lL][oO][aA][dD][eE][rR]11")
    target_add_wic_texture_loader(${TARGET})
  endif()
endfunction(target_add_libraries)

function(update_sources_msvc SOURCES)
  foreach(_source IN ITEMS ${SOURCES})
    if(IS_ABSOLUTE "${_source}")
      file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
    else()
      set(_source_rel "${_source}")
    endif()

    get_filename_component(_source_path "${_source_rel}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
  endforeach()
endfunction(update_sources_msvc)