#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {

    auto Texture = new DSTexture();
    Texture->LoadFromFile("d:/test/texture.dst");
    const auto textureInfo = DSString::Format("Texture size %dx%d", Texture->GetWidth(), Texture->GetHeight());
    Debug::Log(textureInfo);


    Debug::Log("Game initialized.");

    auto engine = new Engine();

    // Main game loop.
    if (engine->Init("Dark Shadows",1280, 720)) {
        while (engine->IsRunning()) {
            engine->Frame();
        }
    }


    Debug::Log("Game execution finished.");

    return 0;
}