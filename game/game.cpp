#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {
    float a = Mathf::Sqrt(12.0f);

    auto vector = Vector3(1.f,2.f,3.f);

    auto pepe = Vector2::zero;

    pepe.x = 12.0f;

    auto lolo = Vector3::zero;

    // Convert PNG to DST
    const auto tex = DSTexture::CreateFromFile("d:/test/001.png");
    tex->GenerateMipmaps();
    tex->CompressToDXT(DSTexture::Format::DXT5);
    auto res = tex->SaveToFile("d:/test/texture.dst");

    const auto tex1 = new DSTexture();
    tex1->LoadFromFile("d:/test/texture.dst");

    const auto textureInfo = DSString::Format("Texture size %dx%d",tex1->GetWidth(),tex1->GetHeight());

    Debug::Log(textureInfo);

    Debug::Log("Engine initialized.");

    auto engine = new Engine();

    // Main game loop.
    if (engine->Init("Dark Shadows",1280, 720)) {
        while (engine->IsRunning()) {
            engine->Frame();
        }
    }

    Debug::Log("Engine execution finished.");

    return 0;
}