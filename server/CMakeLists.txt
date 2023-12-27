option(test "build all tests." ON)
set(SERVER_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/server")
file(GLOB_RECURSE SERVER_SOURCES ${SERER_SOURCES_DIR}/*.*)

foreach(_source IN ITEMS ${SERVER_SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()

  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(server ${SERVER_SOURCES})
target_link_libraries(server engine)
target_add_libraries(server "mal-packet-weaver" "mal-toolkit")

target_include_directories(server PRIVATE ${SERVER_SOURCES_DIR})

set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})