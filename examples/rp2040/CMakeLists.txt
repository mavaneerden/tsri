cmake_minimum_required(VERSION 3.13)

# Set the PICO_TOOLCHAIN_PATH to point to the correct compiler toolchain
get_filename_component(ABS_PATH_TOOLCHAIN "../toolchain"
                       REALPATH BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
unset(PICO_TOOLCHAIN_PATH CACHE)
set(PICO_TOOLCHAIN_PATH "${ABS_PATH_TOOLCHAIN}")

# initialize pico-sdk from submodule
# note: this must happen before project()
include(../pico-sdk/pico_sdk_init.cmake)

project(tsri_rp2040_example)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add TSRI. We need a dummy binary directory here because the example project is a subfolder.
# Normally, you shouldn't need to add the binary directory.
add_subdirectory(${TSRI_DIRECTORY} ${CMAKE_CURRENT_LIST_DIR}/tsri_build)

add_executable(${PROJECT_NAME} blink.cpp)

# Target the tsri library to include all the files :)
target_link_libraries(${PROJECT_NAME} pico_stdlib tsri)

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_COMPILE_COMMANDS ON)
