set(I686PATH "/home/user/opt/cross/bin/i686-elf-")
set(ARCHDIR "/home/user/opt/cross/lib/gcc/i686-elf/11.4.0")

set(CRTI_O "${CMAKE_CURRENT_BINARY_DIR}/crti.o")
set(CRTBEGIN_O "${ARCHDIR}/crtbegin.o")
set(CRTEND_O "${ARCHDIR}/crtend.o")
set(CRTN_O "${CMAKE_CURRENT_BINARY_DIR}/crtn.o")

set(CMAKE_AR                    "${I686PATH}ar")
set(CMAKE_ASM_COMPILER          "${I686PATH}as")
set(CMAKE_C_COMPILER            "${I686PATH}gcc")
set(CMAKE_CXX_COMPILER          "${I686PATH}c++")
set(CMAKE_LINKER                "${I686PATH}gcc")
set(CMAKE_OBJCOPY               "${I686PATH}objcopy")
set(CMAKE_RANLIB                "${I686PATH}ranlib")
set(CMAKE_SIZE                  "${I686PATH}size")
set(CMAKE_STRIP                 "${I686PATH}strip")

set(CMAKE_SYSTEM_NAME           "Generic")
set(CMAKE_SYSTEM_PROCESSOR      "x86")

set(CMAKE_C_FLAGS               "-std=c11 -ffreestanding")
set(CMAKE_C_FLAGS_DEBUG         "-g")
set(CMAKE_C_FLAGS_RELEASE       "")

set(CMAKE_EXE_LINKER_FLAGS      "-ffreestanding -nostdlib -g")

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> ${CRTI_O} ${CRTBEGIN_O} <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${CRTEND_O} ${CRTN_O}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM     NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY     ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE     ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE         STATIC_LIBRARY)
# set(CMAKE_C_COMPILER_WORKS 1)
