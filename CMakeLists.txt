cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 17)

project(tmpl CXX)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(tmpl)
add_subdirectory(tmpl-script)

