#pragma once
#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include "DSString.h"
#include "Debug.h"
#include "ds_math.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

using DSEngine::DSString;
using DSEngine::Debug;
using DSEngine::Vector2;
using DSEngine::Vector3;
using DSEngine::Vector4;
using DSEngine::Quaternion;
using DSEngine::Matrix4x4;

namespace DSEngine {
    class Engine {

    public:

        bool Init(DSString title, int width, int height);
        void Run();
        int GetWidth();
        int GetHeight();
    private:
        SDL_Window* m_window;

        int m_width, m_height;

        DSString m_title = "DSENGINE";
    };
}