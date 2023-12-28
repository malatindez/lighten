set(REFLECTION_SOURCES_DIR "${CURRENT_LIST_DIR}")
file(GLOB_RECURSE REFLECTION_SOURCES ${REFLECTION_SOURCES_DIR}/*.*)

update_sources_msvc("${CMAKE_CURRENT_LIST_DIR}" "${REFLECTION_SOURCES}")

add_executable(reflection ${REFLECTION_SOURCES})
target_link_libraries(reflection engine)

target_include_directories(reflection PRIVATE ${REFLECTION_SOURCES_DIR})

set_target_properties(reflection PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})