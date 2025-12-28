cmake_minimum_required(VERSION 3.23)

project(gloom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(prog)

target_sources(prog
  PRIVATE
    src/main.cpp
    src/stb.cpp
    src/glad.c
)

target_include_directories(prog
  PRIVATE
    include
    resources
)

find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

target_link_libraries(prog PRIVATE
    OpenGL::GL
    SDL2::SDL2
    SDL2::SDL2main
    assimp::assimp
    glad::glad
)
