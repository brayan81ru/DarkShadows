#include "DSEngineCore.h"

namespace DSEngine {
    bool DSEngineCore::InitOld(DSString title, int width, int height) {

        // Init the DStime.
        DSTime::Init();

        // SDL Init
        SDL_Init(SDL_INIT_VIDEO);

        m_title = title;

        m_width = width;

        m_height = height;

        m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_SHOWN);


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
        bgfx::setDebug(BGFX_DEBUG_TEXT);

        m_isRunning = true;



        // Rest of initialization...
        return true;
    }

    bool DSEngineCore::Init(DSString title, int width, int height) {

        // Init the DStime.
        DSTime::Init();

        // SDL Init
        SDL_Init(SDL_INIT_VIDEO);

        m_title = title;

        m_width = width;

        m_height = height;

        m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_SHOWN);

        // Initialize renderer
        RendererConfig config = {};
        config.nativeWindowHandle = m_window;
        config.width = 1280;
        config.height = 720;
        config.enableValidationLayers = true;

        std::unique_ptr<DSBaseRenderer> renderer = std::make_unique<DSVulkanRenderer>();
        if (!renderer->Init(config)) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Renderer init failed!", m_window);

            return false;
        }

        m_isRunning = true;

        // Rest of initialization...
        return true;
    }

    void DSEngineCore::ProcessEvents() {
        while (SDL_PollEvent(&m_sdlEvent)) {
            if (m_sdlEvent.type == SDL_QUIT) {
                m_isRunning = false;
            }
        }
    }

    void DSEngineCore::FrameOld(float deltaTime) {
        // Set view 0 clear state.
        bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
            );

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));

        bgfx::touch(0);

        bgfx::dbgTextClear();

        bgfx::dbgTextPrintf(1, 1, 0x0a, "FPS: %.2f",DSTime::GetFPS());

        bgfx::dbgTextPrintf(1, 2, 0x0f, "Frame Time: %.2f ms", DSTime::GetFrameTimeMS());

        bgfx::frame();
    }

    void DSEngineCore::Frame(float deltaTime) {

    }

    int DSEngineCore::GetWidth() {
        return m_width;
    }

    int DSEngineCore::GetHeight() {
        return m_height;
    }

    bool DSEngineCore::Run() {
        ProcessEvents();
        DSTime::Update();
        return m_isRunning;
    }
}
