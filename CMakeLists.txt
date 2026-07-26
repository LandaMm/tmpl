cmake_minimum_required(VERSION 3.20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cheetah)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/node/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/basics/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/basics/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/node/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/node/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/.hpp"
)

add_executable(cheetah ${SOURCES})

target_include_directories(cheetah PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cheetah PROPERTY CXX_STANDARD 20)
endif()

