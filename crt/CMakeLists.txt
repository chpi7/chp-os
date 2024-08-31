# Have to build like this because the path needs to be known for the toolchain file

separate_arguments(out UNIX_COMMAND "${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/crti.s -o ${CMAKE_CURRENT_BINARY_DIR}/crti.o")
add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/crti.o"
    COMMAND ${out}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/crti.s"
)
add_custom_target(crti DEPENDS "${CMAKE_BINARY_DIR}/crti.o")

separate_arguments(out UNIX_COMMAND "${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/crtn.s -o ${CMAKE_CURRENT_BINARY_DIR}/crtn.o")
add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/crtn.o"
    COMMAND ${out}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/crtn.s"
)
add_custom_target(crtn DEPENDS "${CMAKE_BINARY_DIR}/crtn.o")
