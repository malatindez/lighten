set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

set(ENTT_DIR "${LIB_DIR}/entt")
set(GTEST_DIR "${LIB_DIR}/google/google_test")
set(BENCHMARK_DIR "${LIB_DIR}/google/google_benchmark")
set(SPDLOG_DIR "${LIB_DIR}/spdlog")

# entt
add_subdirectory(third_party/entt)

# google test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("${GTEST_DIR}" EXCLUDE_FROM_ALL)

# google benchmark
add_subdirectory("${BENCHMARK_DIR}" EXCLUDE_FROM_ALL)

# spdlog
add_subdirectory("${SPDLOG_DIR}" EXCLUDE_FROM_ALL)

function(target_add_entt TARGET)
  target_link_libraries("${TARGET}" EnTT::EnTT)
endfunction(target_add_entt)

function(target_add_gtest TARGET)
  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  include_directories(${INCLUDES})
  target_link_libraries("${TARGET}" gtest gtest_main)
endfunction(target_add_gtest)

function(target_add_benchmark TARGET)
  target_link_libraries("${TARGET}" benchmark)
endfunction(target_add_benchmark)

function(target_add_spdlog TARGET)
  target_include_directories("${TARGET}" PRIVATE "${SPDLOG_DIR}/include")
endfunction(target_add_spdlog)

function(target_add_libraries TARGET)
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