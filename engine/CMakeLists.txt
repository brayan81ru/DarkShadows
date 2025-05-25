# Engine sources
file(GLOB ENGINE_SOURCES src/*.cpp src/*.h)
add_library(engine STATIC ${ENGINE_SOURCES})

#SDL2 configuration
target_include_directories(engine PUBLIC ${SDL_INCLUDE_DIR})

target_link_directories(engine PUBLIC ${SDL_LIB_DIR})

# Link debug libraries (remove -d for release build)
find_package(SDL2 REQUIRED)
target_link_libraries(engine PUBLIC SDL2::SDL2main)
target_link_libraries(engine PUBLIC SDL2::SDL2)

# BGFX (Vulkan backend)
add_subdirectory(third_party/bgfx EXCLUDE_FROM_ALL)
target_link_libraries(engine PUBLIC bgfx)