cmake_minimum_required (VERSION 2.6)
project (Quiet-Beacon)
include(CheckLibraryExists)
include(CheckIncludeFiles)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DQUIET_DEBUG)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-g -O0")
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-Wl,-no_pie")
else()
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
      set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Ofast -g3")
      set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-Ofast -flto")
    else()
      set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-O2")
    endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wpedantic -Werror -Wall -D_XOPEN_SOURCE=700")
set(SRCFILES src/decoder.c src/encoder.c)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(CORE_DEPENDENCIES LIQUID QUIET)

find_library(LIQUID liquid REQUIRED)
if (LIQUID)
    check_library_exists(LIQUID flexframesync_set_header_len,flexframegen_set_header_len,ofdmflexframesync_set_header_len,ofdmflexframegen_set_header_len,resamp_rrrf_execute_output_block,liquid_pack_array_block "" LIQUID_DEVEL_FUNCTIONS)
    if(NOT LIQUID_DEVEL_FUNCTIONS)
        unset(LIQUID_DEVEL_FUNCTIONS CACHE)
        message(FATAL_ERROR "
libquiet requires the devel branch of libliquid but
  found a different branch installed
fetch the devel branch using
    git clone https://github.com/quiet/liquid-dsp.git -b devel --single-branch
and install it before continuing")
    endif()
else()
    unset(LIQUID CACHE)
    message(FATAL_ERROR "
libquiet requires libliquid but cannot find it
fetch the devel branch using
    git clone https://github.com/quiet/liquid-dsp.git -b devel --single-branch
and install it before continuing")
endif()

add_library(quiet_beacon_static ${SRCFILES})
target_link_libraries(quiet_beacon_static ${CORE_DEPENDENCIES})
add_library(quiet_beacon SHARED ${SRCFILES})
target_link_libraries(quiet_beacon ${CORE_DEPENDENCIES})

add_custom_target(lib DEPENDS quiet_beacon quiet_beacon_static)

add_executable(integration_test_runner EXCLUDE_FROM_ALL tests/integration.c)
target_link_libraries(integration_test_runner quiet_beacon_static)
set_target_properties(integration_test_runner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "tests")
add_test(NAME integration_test WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND integration_test_runner)

add_custom_target(test_runners DEPENDS integration_test_runner)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS test_runners)
enable_testing()
