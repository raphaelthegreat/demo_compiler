cmake_minimum_required(VERSION 3.15...4.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(compiler VERSION 1.0 LANGUAGES CXX)

add_executable(compiler parse_context.h parse_context.cpp lex.yy.cpp compiler.tab.h compiler.tab.cpp main.cpp)
