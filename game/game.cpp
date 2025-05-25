#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {
    Debug::Log("Engine initialized.");
    Debug::LogWarning("The planet is warming...");
    Debug::LogError("The planet is on fire...");

    if (Engine engine{}; engine.Init("Dark Shadows",1280, 720)) {
        engine.Run();
    }

    //Debug::Log("Engine execution finished.");

    return 0;
}