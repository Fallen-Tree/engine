#include "engine.hpp"

static int SCR_HEIGHT = 600;
static int SCR_WIDTH = 800;

glm::vec3 viewPos(0.f, 0.f, 0.f);

int main() {
    auto engine = Engine();
    engine.Run(SCR_WIDTH, SCR_HEIGHT);
}
