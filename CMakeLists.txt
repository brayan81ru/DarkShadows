cmake_minimum_required(VERSION 3.20)

project(DarkShadows)

set(CMAKE_CXX_STANDARD 17)

# Setup SDL 2
set(SDL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/engine/third_party/SDL2-2.32.6")
set(SDL_INCLUDE_DIR "${SFML_PATH}/include")
set(SDL_LIB_DIR "${SFML_PATH}/lib/x64")

add_subdirectory(engine)

add_subdirectory(game)



