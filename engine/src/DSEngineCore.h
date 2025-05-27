#pragma once
#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include "DSString.h"
#include "Debug.h"
#include "DSMath.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "DSTexture.h"
#include "DSTime.h"
#include "DSBaseRenderer.h"
#include "DSVulkanRenderer.h"

using DSEngine::DSString;
using DSEngine::Debug;
using DSEngine::Vector2;
using DSEngine::Vector3;
using DSEngine::Vector4;
using DSEngine::Quaternion;
using DSEngine::Matrix4x4;
using DSEngine::DSTexture;
using DSEngine::DSTime;
using DSEngine::DSBaseRenderer;
using DSEngine::DSVulkanRenderer;

namespace DSEngine {

    class DSEngineCore {
    public:
        bool InitOld(DSString title, int width, int height);
        bool Init(DSString title, int width, int height);
        bool Run();
        void Frame(float deltaTime);
        int GetWidth();
        int GetHeight();
    private:
        void ProcessEvents();

        void FrameOld(float deltaTime);

        SDL_Window* m_window;
        int m_width, m_height;
        DSString m_title = "DSENGINE";
        bool m_isRunning = true;
        SDL_Event m_sdlEvent;
    };

}