cmake_minimum_required(VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cpl-parser)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/node/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/node/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/node/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(cpl-parser ${SOURCES})

target_include_directories(cpl-parser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(cpl-parser cpl-basics)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cpl-parser PROPERTY CXX_STANDARD 20)
  set_property(TARGET cpl-parser PROPERTY CXX_STANDARD_REQUIRED 20)
endif()

