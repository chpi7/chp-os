set(LIBC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(
    libc
    STATIC
    stdio.c
    string.c
)

target_include_directories(libc PUBLIC ${LIBC_INCLUDE_DIR})

target_link_libraries(
    libc
    PUBLIC
    gcc
)
