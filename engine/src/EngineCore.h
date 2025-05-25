#pragma once
#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>

class Engine {
public:
    bool Init(int width, int height);
    void Run();
    int GetWidth();
    int GetHeight();
private:
    SDL_Window* m_window;
    int m_width, m_height;
};