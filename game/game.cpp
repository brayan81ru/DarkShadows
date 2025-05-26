#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {

    float a = Mathf::Sqrt(12.0f);
    auto pepe = Vector2::zero;
    auto vector3 = Vector3::zero;

    // Convert PNG to DST
    DSTexture* tex = DSTexture::CreateFromFile("d:/test/001.png");
    //-tex->SaveToFile("d:/test/texture.dst");


    Debug::Log("Engine initialized.");

    if (Engine engine{}; engine.Init("Dark Shadows",1280, 720)) {
        engine.Run();
    }

    Debug::Log("Engine execution finished.");

    return 0;
}