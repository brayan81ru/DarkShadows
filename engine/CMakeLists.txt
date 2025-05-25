# Engine library
add_library(engine STATIC src/EngineCore.cpp src/EngineCore.h)

#SDL2 configuration
target_include_directories(engine PUBLIC ${SDL_INCLUDE_DIR})

target_link_directories(engine PUBLIC ${SDL_LIB_DIR})

# Link debug libraries (remove -d for release build)
find_package(SDL2 REQUIRED)
target_link_libraries(engine PUBLIC SDL2::SDL2)