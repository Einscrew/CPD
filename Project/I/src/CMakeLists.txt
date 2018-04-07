cmake_minimum_required(VERSION 3.10)
project(src C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(src
        board.c
        board.h
        sudoku-omp.c
        sudoku-omp.h)
