#define SDL_MAIN_HANDLED
#include "../engine/src/EngineCore.h"

int main() {

    const auto pepe = new dark_shadows_engine::GameObject();

    pepe->m_name = "pepe";

    if (Engine engine{}; engine.Init(1280, 720)) {
        engine.Run();
    }

    return 0;
}