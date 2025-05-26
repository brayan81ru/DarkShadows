#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::DSEngineCore;

int main() {
    auto Texture = new DSTexture();
    Texture->LoadFromFile("d:/test/texture.dst");
    const auto textureInfo = DSString::Format("Texture size %dx%d", Texture->GetWidth(), Texture->GetHeight());
    Debug::Log(textureInfo);

    Debug::Log("Game initialized.");

    auto DSEngine = new DSEngineCore();

    // Main game loop.
    if (DSEngine->Init("Dark Shadows",1280, 720)) {
        while (DSEngine->Run()) {
            DSEngine->Frame(DSTime::GetDeltaTime());
        }
    }

    Debug::Log("Game execution finished.");

    return 0;
}