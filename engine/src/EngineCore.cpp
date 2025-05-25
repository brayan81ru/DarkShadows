#include "EngineCore.h"

bool Engine::Init(int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);
    m_window = SDL_CreateWindow("Dark Shadows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    return true;
}

void Engine::Run() {
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
    }
}