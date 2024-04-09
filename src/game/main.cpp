#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "logger.hpp"

#include "classes.cpp"
#include "pool.cpp"
#include "player_controller.cpp"

// Made this global for easier usage inside functions
Engine *engine;

// I'd like to move responsibility about default shader to engine
ShaderProgram *defaultSP;

// Initializing global variables
void init() {
    engine = new Engine();
    std::string vertexShaderSource = "standart.vshader";
    std::string fragmentShaderSource = "standart.fshader";
    Shader vShader = Shader(VertexShader, vertexShaderSource);
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);
    defaultSP = new ShaderProgram(vShader, fShader);
}

void createUI() {
    class FpsText : public Behaviour {
     public:
        void Update(float dt) override {
            int fps = Time::GetCurrentFps();
            char buf[12];
            snprintf(buf, sizeof(buf), "Fps: %d", fps);
            self.GetText()->SetContent(buf);
        }
    };

    auto ocraFont = new Font("OCRAEXT.TTF", 20);
    auto obj = engine->NewObject();
    obj.AddText(ocraFont, "", 0.85f, 0.95f, 1.f, Vec3(0, 0, 0));
    // obj.AddText(ocraFont, "", 685.0f, 575.0f, 1.f, Vec3(0, 0, 0));
    obj.AddBehaviour<FpsText>();

    // engine->NewObject().AddImage("hp.png", 0.03f, 0.15f, 0.4f);
    // engine->NewObject().AddImage("hp_bar.png", 0.015f, 0.01f, 0.4f);
}

void createLights() {
    engine->NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.0, 0.0, 0.0),
        1.f, 0.0f, 0.0f);
    /*
    engine->NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(2.3f, -3.3f, -4.0f),
        1.f, 0.09f, 0.032f);

    engine->NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f,  0.0f, -3.0f),
        1.f, 0.09f, 0.032f);

    engine->NewObject().AddDirLight(
        Vec3(0.05f, 0.05f, 0.05f), Vec3(0.4f, 0.4f, 0.4f),
        Vec3(0.5f, 0.5f, 0.5f),  Vec3(-0.2f, -1.0f, -0.3f));
    */
    engine->NewObject().AddSpotLight(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f),
        1.0f, 0.09f, 0.032f, Vec3(0),
        glm::cos(glm::radians(12.5f)),
        glm::cos(glm::radians(15.0f)));
    /*
    engine->NewObject().AddSpotLight(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(0.3f, 0.3f, .3f), Vec3(1.0f, 1.0f, 1.0f),
        1.0f, 0.09f, 0.032f, Vec3(0),
        glm::cos(glm::radians(10.0f)),
        glm::cos(glm::radians(20.0f)));
    */
}

void addCat() {
    Model * model = Model::loadFromFile("cat.obj");
    model->shader = defaultSP;
    Material cat_material = {
        4.f,
        Texture("/Cat_diffuse.png", "/Cat_specular.png")
    };
    model->setMaterial(cat_material);
    auto obj = engine->NewObject();
    obj.AddModel(*model);
    auto &t = obj.AddTransform(Vec3(0.f, -3.f, -8.f), Vec3(.1f, .1f, .1f), Mat4(1.0));
    t.Rotate(1.67f, Vec3(-1.f, 0.f, 0.f));
    obj.AddCollider(Collider::GetDefaultAABB(&model->meshes[0]));
    // obj.AddRigidBody(100.f, Mat3(0), Vec3(0), 0.f, Vec3(0, -1000, 0),
    //     Vec3(0), Vec3(1), 0.1f);
}

void buildRoom() {
    /*Model *chair = Model::loadFromFile("Chair.obj");
    // chair->setMaterial({ 4.f, Texture("Chair_BaseColor.png") });
    Transform *chTransform = new Transform(Vec3(20, -6, 5), Vec3(0.4), Mat4(1.0));
    Object chairObj = newModel(chTransform, chair);

    Model *table = Model::loadFromFile("Table Round Small.glb");
    Transform *tableTransform = new Transform(Vec3(20, -6, 10), Vec3(1), Mat4(1.0));
    Object tableObj = newModel(tableTransform, table);

    Model *bar = Model::loadFromFile("bar/Bar.obj");
    // bar->setMaterial({4.f, Texture("Bar_A_BaseColor.png")});
    Transform *barTransform = new Transform(Vec3(5, -8, -1), Vec3(2), Mat4(1.0));
    Object barObj = newModel(barTransform, bar);
*/

    Model *wall = Model::loadFromFile("huge_cube.obj");
    wall->setMaterial({4.f, Texture("stone_wall.png")});
    newModel(new Transform(Vec3(0, 0, 0), Vec3(20), Mat4(1.0)), wall);

    Model *cc = Model::loadFromFile("CompanionCube/Portal_Companion_Cube.obj");
    newModel(new Transform(Vec3(8, 0, 0), Vec3(1), Mat4(1.0)), cc);

    Model *plant1 = Model::loadFromFile("HousePlant/Houseplant.obj");
    newModel(new Transform(Vec3(10, -2, 0), Vec3(1), Mat4(1.0)), plant1);

    Model *plant2 = Model::loadFromFile("Orchid/Orchid.obj");
    newModel(new Transform(Vec3(14, -2, 0), Vec3(0.3), Mat4(1.0)), plant2);

    //Model *dog = Model::loadFromFile("ShibaInu.fbx");
    //newModel(new Transform(Vec3(14, -2, 4), Vec3(1), Mat4(1.0)), dog);
}

void poolTable() {

    float balls_y = 2;
    std::vector<Vec3> coordinates {
    /*    Vec3(-2, balls_y, -0.2f),
        Vec3(-2.5f, balls_y, -0.5f),
        Vec3(-2.5f, balls_y, 0.1f),
        Vec3(-3.f, balls_y, -0.8f),
        Vec3(-3.f, balls_y, -0.2f),
        Vec3(-3.f, balls_y, 0.4f),
        Vec3(-3.5f, balls_y, -1.1f),
        Vec3(-3.5f, balls_y, -0.5f),
        Vec3(-3.5f, balls_y, 0.1f),
        Vec3(-3.5f, balls_y, 0.7f),
        Vec3(-4, balls_y, -1.4f),
        Vec3(-4, balls_y, -0.8f),
        Vec3(-4, balls_y, -0.2f),
        Vec3(-4, balls_y, 0.4f),
        Vec3(-4, balls_y, 1.f),
    */
        Vec3(2, balls_y, -0.2f),
    };

    int ballsCount = coordinates.size();
    std::vector<MovingBall*> balls;
    for (int i = 0; i < ballsCount; i++) {
        Vec3 pos = coordinates[i];
        auto newBall = MovingBall::New(pos, 0.2f, "pool/" + std::to_string(i % 16 + 1) + ".png");
        balls.push_back(reinterpret_cast<MovingBall*>(newBall.GetBehaviour()));
    }

    Table::New(Vec3(0, -7, 0), Vec3(5));

    Cue::New(balls, engine->camera);
}

int main() {
    // Logger::SetLoggingLevel(WARN);
    init();

    buildRoom();
    poolTable();
    // addCat();
    createUI();
    createLights();

    Object player = engine->NewObject();
    player.AddBehaviour<PlayerController>(engine->camera);
    player.AddTransform(Vec3(0), Vec3(1), Mat4(1));

    engine->Run();
}
