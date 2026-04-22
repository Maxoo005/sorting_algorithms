cmake_minimum_required(VERSION 3.16)
project(AiZO_Projekt1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror)

# Dodaj plik wykonywalny
add_executable(${PROJECT_NAME}
    src/main.cpp
)
