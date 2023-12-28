set(SERVER_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/server")
file(GLOB_RECURSE SERVER_SOURCES ${SERVER_SOURCES_DIR}/*.*)

update_sources_msvc("${CMAKE_CURRENT_LIST_DIR}" "${SERVER_SOURCES}")

add_executable(server ${SERVER_SOURCES})
target_link_libraries(server engine)

target_include_directories(server PRIVATE ${SERVER_SOURCES_DIR})

set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})