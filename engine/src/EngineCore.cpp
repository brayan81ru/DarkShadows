#include "EngineCore.h"

bool Engine::Init(const int width, const int height) {
    // SDL Init
    SDL_Init(SDL_INIT_VIDEO);
    m_window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

    // BGFX Platform Data
    bgfx::PlatformData pd;
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);

    if (!SDL_GetWindowWMInfo(m_window, &wmi)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "SDL_GetWindowWMInfo failed: %s", SDL_GetError());
        return false;
    }

    #if defined(_WIN32)
    pd.nwh = wmi.info.win.window;
    #elif defined(__linux__)
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
    #endif

    bgfx::Init bgfxInit;
    bgfxInit.platformData = pd;
    bgfxInit.type = bgfx::RendererType::Vulkan;  // Or Direct3D11/OpenGL
    bgfx::init(bgfxInit);

    // Rest of initialization...
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