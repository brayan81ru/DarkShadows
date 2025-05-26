#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {

    float a = Mathf::Sqrt(12.0f);

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

    if (Engine engine{}; engine.Init("Dark Shadows",1280, 720)) {
        engine.Run();
    }

    Debug::Log("Engine execution finished.");

    return 0;
}