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

set(SRC_DIR kernel)

set(LINKSCRIPT ${CMAKE_SOURCE_DIR}/${SRC_DIR}/linker.ld)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKSCRIPT}")

set(SOURCES
    ${SRC_DIR}/term.c
    ${SRC_DIR}/kernel.c
    ${SRC_DIR}/printf.c
    ${SRC_DIR}/memory.c
    ${SRC_DIR}/ps2.c
)

include_directories(${SRC_DIR}/include)

separate_arguments(out UNIX_COMMAND "${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/crti.s -o ${CMAKE_CURRENT_BINARY_DIR}/crti.o")
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/crti.o"
    COMMAND ${out}
    DEPENDS "${SRC_DIR}/crti.s"
)
add_custom_target(crti DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/crti.o")

separate_arguments(out UNIX_COMMAND "${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/crtn.s -o ${CMAKE_CURRENT_BINARY_DIR}/crtn.o")
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/crtn.o"
    COMMAND ${out}
    DEPENDS "${SRC_DIR}/crtn.s"
)
add_custom_target(crtn DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/crtn.o")

add_executable(
    mykernel.elf
    ${SRC_DIR}/start.s
    ${SOURCES}
)
add_dependencies(
    mykernel.elf
    crti
    crtn
)

message("CMAKE_C_LINK_EXECUTABLE=${CMAKE_C_LINK_EXECUTABLE}")

target_link_libraries(
    mykernel.elf
    gcc
)
set_target_properties(mykernel.elf PROPERTIES LINK_DEPENDS ${LINKSCRIPT})


add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/mykernel.iso"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/package-iso.sh" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS mykernel.elf
)
add_custom_target(iso DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/mykernel.iso")

