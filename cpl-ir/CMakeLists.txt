cmake_minimum_required(VERSION 3.20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cpl-ir)

add_library(cpl-ir
	"${CMAKE_CURRENT_SOURCE_DIR}/src/instr.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/block.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ir.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/type.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/cplbuild.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/symbol.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/type.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/value.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/function.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/scope.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/instr.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/instr/call.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/instr/alloca.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/instr/store.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/instr/load.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/block.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/cpl-ir/ir.h"
)

target_include_directories(cpl-ir PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(cpl-ir cpl-parser)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cpl-ir PROPERTY CXX_STANDARD 20)
  set_property(TARGET cpl-ir PROPERTY CXX_STANDARD_REQUIRED 20)
endif()
