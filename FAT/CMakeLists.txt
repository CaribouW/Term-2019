cmake_minimum_required(VERSION 3.14)
set(CMAKE_ASM_NASM_OBJECT_FORMAT macho64)
set(CMAKE_CXX_STANDARD 11)
enable_language(ASM_NASM)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS}  --prefix _")
project(FAT)
add_executable(FAT
        my_print.asm
        main.cpp
        fatFile/fat.h
        engine/InstrEngine.cpp
        engine/InstrEngine.h
        fatFile/fat.cpp)