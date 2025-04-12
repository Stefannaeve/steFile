cmake_minimum_required(VERSION 3.30)
project(steFile C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(steFile
        src/main.c
        include/main.h
        src/windowApplicationSteFile.c
        include/steFile.h
        src/createSteFile.c
        include/createSteFile.h
        src/SNLogger.c
        include/SNLogger.h
        include/windowApplicationSteFile.h)

target_link_libraries(steFile ${SDL2_LIBRARIES})