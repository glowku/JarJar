# Windows Toolchain for JARJARVIS
# CMake toolchain file for building on Windows

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Prevent CMake from adding system-specific flags
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Find cross-compiler
find_program(ELF_GCC x86_64-elf-gcc)
find_program(ELF_GXX x86_64-elf-g++)
find_program(ELF_LD x86_64-elf-ld)
find_program(ELF_AR x86_64-elf-ar)
find_program(ELF_OBJCOPY x86_64-elf-objcopy)
find_program(ELF_OBJDUMP x86_64-elf-objdump)
find_program(ELF_NM x86_64-elf-nm)
find_program(ELF_RANLIB x86_64-elf-ranlib)

if(ELF_GCC)
    set(CMAKE_C_COMPILER ${ELF_GCC})
    set(CMAKE_CXX_COMPILER ${ELF_GXX})
    set(CMAKE_LINKER ${ELF_LD})
    set(CMAKE_AR ${ELF_AR})
    set(CMAKE_OBJCOPY ${ELF_OBJCOPY})
    set(CMAKE_OBJDUMP ${ELF_OBJDUMP})
    set(CMAKE_NM ${ELF_NM})
    set(CMAKE_RANLIB ${ELF_RANLIB})
    message(STATUS "Using x86_64-elf toolchain")
else()
    # Fallback to MinGW
    find_program(MINGW_GCC x86_64-w64-mingw32-gcc)
    find_program(MINGW_GXX x86_64-w64-mingw32-g++)
    
    if(MINGW_GCC)
        set(CMAKE_C_COMPILER ${MINGW_GCC})
        set(CMAKE_CXX_COMPILER ${MINGW_GXX})
        message(STATUS "Using MinGW-w64 toolchain")
        message(WARNING "MinGW may not produce valid kernel binaries. Consider installing x86_64-elf-gcc.")
    else()
        message(FATAL_ERROR "No suitable cross-compiler found!")
    endif()
endif()

# NASM
find_program(CMAKE_ASM_NASM_COMPILER nasm)
if(NOT CMAKE_ASM_NASM_COMPILER)
    message(FATAL_ERROR "NASM not found!")
endif()

# Compiler flags for freestanding kernel
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -O2 -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions -fno-rtti -fno-stack-protector")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-red-zone -mno-mmx -mno-sse -mno-sse2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcmodel=large -m64")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -O2 -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-red-zone -mno-mmx -mno-sse -mno-sse2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=large -m64")

set(CMAKE_ASM_NASM_FLAGS "-f elf64")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -nostdlib")

# Don't search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
