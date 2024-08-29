cmake_minimum_required(VERSION 3.22)

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
)

include_directories(${SRC_DIR}/include)

add_executable(
    mykernel.elf
    ${SRC_DIR}/start.s
    ${SOURCES}
)

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

