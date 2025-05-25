#define SDL_MAIN_HANDLED

#include "../engine/src/EngineCore.h"

using DSEngine::Engine;

int main() {


    if (Engine engine{}; engine.Init(1280, 720)) {
        engine.Run();
    }
    return 0;
}