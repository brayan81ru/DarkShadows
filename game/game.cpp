#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {
    const DSEngine::DSString pepe = "pepe";

    auto len = pepe.Length();

    auto title = DSEngine::DSString::Format("Dark %f ",12.f);

    if (Engine engine{}; engine.Init(title,1280, 720)) {
        engine.Run();
    }
    return 0;
}