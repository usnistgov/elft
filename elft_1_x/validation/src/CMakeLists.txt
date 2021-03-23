# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.

cmake_minimum_required(VERSION 3.11)

project(elft_validation)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(elft_validation)
target_sources(elft_validation PRIVATE elft_validation.cpp)
target_include_directories(elft_validation PRIVATE .)
target_include_directories(elft_validation PUBLIC ../../include)

# Build libelft first
add_subdirectory(${PROJECT_SOURCE_DIR}/../../libelft ${CMAKE_CURRENT_BINARY_DIR}/libelft)
add_dependencies(elft_validation elft)
target_link_libraries(elft_validation PUBLIC elft)

# Find the participant's core library
# Writes CORE_LIB as the name of the library in the parent scope
function(find_core_library)
	set(CORE_LIB_PREFIX "libelft_")
	file(GLOB CORE_LIB_POSSIBILITIES
	    LIST_DIRECTORIES true
	    RELATIVE ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
	    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CORE_LIB_PREFIX}*_*${CMAKE_SHARED_LIBRARY_SUFFIX})

	foreach (lib ${CORE_LIB_POSSIBILITIES})
		if(CORE_LIB)
			message(FATAL_ERROR "More than one core library found in ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}. Please review ELFT naming conventions.")
		endif()

		string(REGEX MATCH "${CORE_LIB_PREFIX}[A-Za-z0-9]+_[0-9A-F][0-9A-F][0-9A-F][0-9A-F]${CMAKE_SHARED_LIBRARY_SUFFIX}" CORE_LIB ${lib})
	endforeach()

	set(CORE_LIB ${CORE_LIB} PARENT_SCOPE)
endfunction()

find_core_library()
if(NOT CORE_LIB)
	message(FATAL_ERROR "No core library found in ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}. Please review ELFT naming conventions.")
else()
	message(STATUS "Found core library: ${CORE_LIB}")
	target_link_libraries(elft_validation PUBLIC ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CORE_LIB})
endif()

# GCC < 9 needs to explicitly link libstdc++fs
target_link_libraries(elft_validation PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)

# Extern the version symbols
target_compile_definitions(elft_validation PRIVATE NIST_EXTERN_API_VERSION)

# Turn on warnings
target_compile_options(elft_validation PRIVATE
    -Wall -Wextra -pedantic -Wconversion -Wsign-conversion)

# Set RPATH
set_target_properties(elft_validation PROPERTIES
    # Set RPATH to installed version of libs
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

# Don't also include local copy of libs in RPATH
set_target_properties(elft_validation PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE)

# Use RUNPATH over RPATH
set_target_properties(elft_validation PROPERTIES
    LINK_FLAGS -Wl,--enable-new-dtags)

install(TARGETS elft_validation
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/../${CMAKE_INSTALL_BINDIR})
