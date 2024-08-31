set(LINKSCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/src/arch/i686/linker.ld)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKSCRIPT}")

set(SOURCES
    src/kernel.c
    src/io/tty/tty.c
    src/io/ps2/ps2.c
    src/arch/i686/start.s
    src/io/arch/i686/inout.c
)

add_executable(
    mykernel.elf
    ${SOURCES}
)

target_include_directories(mykernel.elf PUBLIC src)

add_dependencies(
    mykernel.elf
    crti
    crtn
)

message("CMAKE_C_LINK_EXECUTABLE=${CMAKE_C_LINK_EXECUTABLE}")

target_link_libraries(
    mykernel.elf
    PUBLIC
    gcc
    libc
)
set_target_properties(mykernel.elf PROPERTIES LINK_DEPENDS ${LINKSCRIPT})

