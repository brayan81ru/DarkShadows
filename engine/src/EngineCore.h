#pragma once

#include <SDL.h>

class Engine {
public:
    bool Init(int width, int height);
    void Run();
private:
    SDL_Window* m_window;
};