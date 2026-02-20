# libc CMakeLists.txt

set(LIBC_SOURCES
    src/string.c
    src/stdlib.c
    src/stdio.c
    src/math.c
    src/time.c
    src/ctype.c
    src/errno.c
)

add_library(libc STATIC ${LIBC_SOURCES})

target_include_directories(libc PUBLIC
    ${CMAKE_SOURCE_DIR}/libc/include
    ${CMAKE_SOURCE_DIR}/kernel/include
)

target_compile_options(libc PRIVATE
    -ffreestanding
    -nostdlib
    -fno-builtin
    -fno-stack-protector
)
