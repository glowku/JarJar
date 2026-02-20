# Tools CMakeLists.txt

set(TOOLS_SOURCES
    src/shell.c
)

add_library(tools STATIC ${TOOLS_SOURCES})

target_include_directories(tools PUBLIC
    ${CMAKE_SOURCE_DIR}/tools/include
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/libc/include
    ${CMAKE_SOURCE_DIR}/ai_core/include
    ${CMAKE_SOURCE_DIR}/filesystem/include
)

target_compile_options(tools PRIVATE
    -ffreestanding
    -nostdlib
)
