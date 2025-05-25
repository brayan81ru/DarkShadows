add_executable(dark_shadows game.cpp)

target_link_libraries(dark_shadows PRIVATE engine)

# Copy SFML DLLs automatically
add_custom_command(TARGET dark_shadows POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy
        "${SDL_PATH}/lib/x64/SDL2.dll"
        $<TARGET_FILE_DIR:dark_shadows>
)