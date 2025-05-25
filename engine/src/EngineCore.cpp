#include "EngineCore.h"

bool Engine::Init(int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);
    m_window = SDL_CreateWindow("Diablo Clone", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width, height, 0);

    bgfx::PlatformData pd;
    pd.nwh = SDL_GetWindowWMInfo(m_window)->info.win.window;
    bgfx::init(pd);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff);
    return true;
}

void Engine::Run() {
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        bgfx::frame();
    }
}