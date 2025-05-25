#define SDL_MAIN_HANDLED
#include "../engine/src/EngineCore.h"

int main() {
    Engine engine;
    if (engine.Init(1280, 720)) {
        engine.Run();
    }
    return 0;
}