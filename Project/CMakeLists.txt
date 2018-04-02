cmake_minimum_required(VERSION 3.10)
project(Project)

set(CMAKE_CXX_STANDARD 11)

include_directories(I/src)

add_executable(Project
        I/src/board.c
        I/src/board.h
        I/src/main.c
        I/src/stack.c
        I/src/stack.h
        I/src/strategies.c
        I/src/strategies.h
        I/src/utils.c
        I/src/utils.h
        I/tests/16x16.txt
        I/tests/2x2.txt
        I/tests/3.txt
        I/tests/9x9.txt
        I/tests/bad.txt
        I/tests/easy.txt
        I/tests/game.txt
        I/tests/out-16.txt
        I/makefile)
