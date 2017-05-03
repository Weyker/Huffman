cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project("Huffman Compressor and Decompressor" CXX C)
 # C is needed for FindThreads

# ---------------------------------------
# Initial config
# ---------------------------------------

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# For external tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------
# Project options
# ---------------------------------------

option(ENABLE_CLANG_MODULES "Enables support for clang modules" OFF)

# ---------------------------------------
# Handle options
# ---------------------------------------

if(ENABLE_CLANG_MODULES)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-fmodules -fcxx-modules)
endif()
endif()

# ---------------------------------------
# Add the CP-utils header-only library
# ---------------------------------------

add_library(Huffman INTERFACE)
target_include_directories(Huffman INTERFACE "include")

# The following list does not contain all features used in CP-utils
# but contain the main ones. It will induce cmake to compile with a C++ standard >= 11
target_compile_features(Huffman INTERFACE
  cxx_auto_type
  cxx_lambdas
  cxx_nullptr
)

add_subdirectory("src")

