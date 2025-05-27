#pragma once
#include <memory>

namespace DSEngine {
    struct RendererConfig {
        void* nativeWindowHandle;  // SDL_Window* converted to platform-specific (Win32/HWND, X11/Window, etc.)
        uint32_t width, height;
        bool enableValidationLayers;
    };

    class DSBaseRenderer {
    public:
        virtual ~DSBaseRenderer() = default;

        // Core methods
        virtual bool Init(const RendererConfig& config) = 0;
        virtual void Clear(float r, float g, float b, float a, bool clearDepth = true) = 0;
        virtual void Frame() = 0;

        // Optional: Resource management
        virtual void Resize(uint32_t width, uint32_t height) = 0;
    };
}