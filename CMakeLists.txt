cmake_minimum_required(VERSION 3.30)
project(steFile C)

set(CMAKE_C_STANDARD 11)

add_executable(steFile
        src/main.c
        include/main.h
        src/steFile.c
        include/steFile.h)
