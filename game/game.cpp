#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {
    Debug::Log("Initializing engine...");

    if (Engine engine{}; engine.Init("Dark Shadows",1280, 720)) {
        engine.Run();
    }

    return 0;
}