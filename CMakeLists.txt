cmake_minimum_required(VERSION 3.22)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/i686-elf-gcc.cmake")
if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(SEND_ERROR "Toolchain file is not there: ${CMAKE_TOOLCHAIN_FILE}")
else()
    message("Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif()

project(
    chp-os
    VERSION 0.1
    LANGUAGES C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(libc)
add_subdirectory(crt)
add_subdirectory(kernel)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/mykernel.iso"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/package-iso.sh" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/kernel"
    DEPENDS mykernel.elf
)
add_custom_target(iso DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/mykernel.iso")

