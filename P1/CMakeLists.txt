cmake_minimum_required(VERSION 3.12)
project(P1)

set(CMAKE_CXX_STANDARD 14)

add_executable(P1
        "Provided Files/code1.txt"
        "Provided Files/code2.txt"
        "Provided Files/code3.txt"
        "Provided Files/code4.txt"
        "Provided Files/definition.txt"
        "Provided Files/output.txt"
        src/definition.txt
        src/main.c
        CMPE322-Project1_definition.pdf)
