cmake_minimum_required(VERSION 3.20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cpl)

add_subdirectory(cpl-basics)
add_subdirectory(cpl-parser)
add_subdirectory(cpl-ir)
add_subdirectory(cpl-codegen)

add_executable(cpl "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

target_include_directories(cpl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(cpl cpl-basics cpl-parser cpl-ir cpl-codegen)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cpl PROPERTY CXX_STANDARD 23)
  set_property(TARGET cpl PROPERTY CXX_STANDARD_REQUIRED 23)
endif()

