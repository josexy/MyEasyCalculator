cmake_minimum_required(VERSION 3.19)
project(calculator)

set(CMAKE_CXX_STANDARD 17)

add_executable(calculator
       Main.cpp
       Calculator/src/ExpressionTree.cc
       Calculator/src/Lexer.cc
        )
