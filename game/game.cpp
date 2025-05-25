#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {
    const DSEngine::DSString pepe = "pepe";

    auto len = pepe.Length();

    if (Engine engine{}; engine.Init("Codename - Dark Shadows",1280, 720)) {
        engine.Run();
    }
    return 0;
}