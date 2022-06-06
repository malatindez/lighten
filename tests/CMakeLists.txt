option(test "build all tests." ON)
if(test)
  set(TEST_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
  file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR} *.*)
  list(FILTER TEST_SOURCES INCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/tests/*")

  foreach(_source IN ITEMS ${TEST_SOURCES})
    if(IS_ABSOLUTE "${_source}")
        file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
    else()
        set(_source_rel "${_source}")
    endif()
    get_filename_component(_source_path "${_source_rel}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
  endforeach()
  
  add_executable(runUnitTests ${TEST_SOURCES} ${ENGINE_SOURCES})
  target_include_directories(runUnitTests PRIVATE ${TEST_SOURCES_DIR})
  target_include_directories(runUnitTests PRIVATE ${ENGINE_SOURCES_DIR})

  target_compile_options(runUnitTests  PRIVATE /bigobj)
  target_add_libraries(runUnitTests "entt" "spdlog" "benchmark" "gtest")

  add_test(NAME TEST COMMAND runUnitTests)

  set_target_properties(runUnitTests PROPERTIES LINKER_LANGUAGE CXX)

endif()
