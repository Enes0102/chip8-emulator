cmake_minimum_required(VERSION 3.16)
project(chip8 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.9
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL2)

add_executable(chip8
    src/main.cpp
    src/chip8.cpp
)

if(TARGET SDL2::SDL2main)
    target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(chip8 PRIVATE SDL2::SDL2-static)
