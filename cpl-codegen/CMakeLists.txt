cmake_minimum_required(VERSION 3.20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cpl-codegen)

add_library(cpl-codegen
	"${CMAKE_CURRENT_SOURCE_DIR}/src/dumper.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/codegen.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/register_allocator.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-codegen/codegen.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-codegen/register_allocator.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-codegen/dumper.h"
)

target_include_directories(cpl-codegen PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(cpl-codegen cpl-ir)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cpl-codegen PROPERTY CXX_STANDARD 20)
  set_property(TARGET cpl-codegen PROPERTY CXX_STANDARD_REQUIRED 20)
endif()
