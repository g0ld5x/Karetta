cmake_minimum_required(VERSION 3.20)

project(karetta
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lexer_test
    src/lexer/lexer.cpp
    src/lexer/lexertest.cpp
)