#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {

    auto pepe = Vector2::zero;

    Debug::Log("Engine initialized.");

    if (Engine engine{}; engine.Init("Dark Shadows",1280, 720)) {
        engine.Run();
    }

    Debug::Log("Engine execution finished.");

    return 0;
}