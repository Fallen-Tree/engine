#include "engine.hpp"

static int SCR_HEIGHT = 600;
static int SCR_WIDTH = 800;

int main() {
    auto engine = Engine();
    engine.Run(SCR_WIDTH, SCR_HEIGHT);
}