cmake_minimum_required(VERSION 3.20)
project(cheetah)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/node/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
)

add_executable(cheetah ${SOURCES})

target_include_directories(cheetah PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
