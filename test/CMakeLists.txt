cmake_minimum_required(VERSION 3.2)

project(UnitTests)

include_directories("../")

set(SOURCES
        "../tools.c"
    )

add_executable(Test main.c ${SOURCES})
