# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.

cmake_minimum_required(VERSION 3.11)

project(elft_output_library)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(elft_output SHARED)
target_sources(elft_output PRIVATE libelft_output.cpp)
target_include_directories(elft_output PRIVATE ../include)

# Extern the version symbols
target_compile_definitions(elft_output PRIVATE NIST_EXTERN_API_VERSION)

# Turn on warnings
target_compile_options(elft_output PRIVATE
    -Wall -Wextra -pedantic -Wconversion -Wsign-conversion)
