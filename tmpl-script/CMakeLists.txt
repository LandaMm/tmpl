
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/interpreter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/node/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
)

add_executable(tmpl-script ${SOURCES})

target_include_directories(tmpl-script PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

