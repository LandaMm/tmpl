cmake_minimum_required(VERSION 3.20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cpl-basics)

add_library(cpl-basics INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-basics/def.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-basics/array.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-basics/string.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-basics/file.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-basics/allocator/arena.hpp"
)

# target_include_directories(cpl-basics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-basics")
target_include_directories(cpl-basics INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cpl-basics PROPERTY CXX_STANDARD 20)
  set_property(TARGET cpl-basics PROPERTY CXX_STANDARD_REQUIRED 20)
endif()
