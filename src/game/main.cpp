#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "logger.hpp"

#include "classes.cpp"
#include "pool.cpp"
#include "player_controller.cpp"

// Made this global for easier usage inside functions
Engine *engine;
Object dogSound;
// I'd like to move responsibility about default shader to engine
ShaderProgram *defaultSP;
ShaderProgram *skeletalSP;

std::vector<Object> interactableObjects(0);

// Initializing global variables
void init() {
    engine = new Engine();
    std::string vertexShaderSource = "standart.vshader";
    std::string skeletalVertexShaderSource = "skeletal.vshader";
    std::string fragmentShaderSource = "standart.fshader";
    defaultSP = new ShaderProgram(
        engine->GetShaderManager().LoadShaderProgram(vertexShaderSource, fragmentShaderSource));
    skeletalSP = new ShaderProgram(
        engine->GetShaderManager().LoadShaderProgram(skeletalVertexShaderSource, fragmentShaderSource));
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

    auto ocraFont = engine->GetFontManager().LoadFont("OCRAEXT.TTF", 20);
    auto obj = engine->NewObject();
    obj.AddText(ocraFont, "", 0.85f, 0.95f, 1.f, Vec3(0, 0, 0));
    obj.AddBehaviour<FpsText>();\

    auto pointer = engine->NewObject();
    pointer.AddText(ocraFont, "+", 0.5f, 0.5f, 1.f, Vec3(1.f, 1.f, 1.f));

    // engine->NewObject().AddImage("hp.png", 0.03f, 0.15f, 0.4f);
    // engine->NewObject().AddImage("hp_bar.png", 0.015f, 0.01f, 0.4f);
}

void AddLantern(Vec3 pos) {
    Model *lantern = engine->GetModelManager().LoadModel("Lantern/one_mesh.obj");
    Transform * tr = new Transform(pos, Vec3(1.0f), Mat4(1.0f));
    newModel(tr, lantern);
    engine->NewObject().AddPointLight(
        Vec3(0.5f, 0.5f, 0.5f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(1.0f, 1.0f, 1.0f), pos,
        1.f, 0.05f, 0.01f);
}

void createLights() {
    engine->NewObject().AddPointLight(
        Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0, 10.0, 0.0),
        1.f, 0.0f, 0.01f);

    engine->NewObject().AddPointLight(
        Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 10.0f, -20.0f),
        1.f, 0.0f, 0.01f);

    engine->NewObject().AddPointLight(
        Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f,  10.0f, 20.0f),
        1.f, 0.0f, 0.01f);
    /*
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

void buildRoom() {
    float floor_y = -6.0f;

    float wall_scale = 4.0f;
    int walls_cnt = 2;
    Vec3 wall_y = Vec3(0, floor_y, 0);
    Model *w2 = engine->GetModelManager().LoadModel("wall.fbx");
    for (int i = -walls_cnt; i <= walls_cnt; ++i) {
        newModel(
            new Transform(Vec3(i, 0, -walls_cnt) * 4.0f * wall_scale + wall_y,
                Vec3(wall_scale), Mat4(1.0)), w2);
        newModel(
            new Transform(Vec3(i, 0, walls_cnt) * 4.0f * wall_scale + wall_y,
                Vec3(wall_scale), Mat4(1.0)), w2)
            .GetTransform()->Rotate(0, glm::radians(180.0f), 0);
        newModel(
            new Transform(Vec3(-walls_cnt, 0, i) * 4.0f * wall_scale + wall_y,
                Vec3(wall_scale), Mat4(1.0)), w2)
            .GetTransform()->Rotate(0, glm::radians(90.0f), 0);
        newModel(
            new Transform(Vec3(walls_cnt, 0, i) * 4.0f * wall_scale + wall_y,
                Vec3(wall_scale), Mat4(1.0)), w2)
            .GetTransform()->Rotate(0, glm::radians(-90.0f), 0);
    }

    float walls_x = walls_cnt * wall_scale * 4.5f + 3.8f;
    newStaticBody(new Transform(Vec3(0), Vec3(1), Mat4(1)), new Collider{AABB {
            Vec3{-walls_x - 1, floor_y, -walls_x},
            Vec3{-walls_x, floor_y + 20, walls_x},
        }});
    newStaticBody(new Transform(Vec3(0), Vec3(1), Mat4(1)), new Collider{AABB {
            Vec3{-walls_x, floor_y, walls_x},
            Vec3{walls_x, floor_y + 10, walls_x + 1},
        }});
    newStaticBody(new Transform(Vec3(0), Vec3(1), Mat4(1)), new Collider{AABB {
            Vec3{walls_x, floor_y, -walls_x},
            Vec3{walls_x + 1, floor_y + 10, walls_x},
        }});
    newStaticBody(new Transform(Vec3(0), Vec3(1), Mat4(1)), new Collider{AABB {
            Vec3{-walls_x, floor_y, -walls_x - 1},
            Vec3{walls_x, floor_y + 10, -walls_x},
        }});
    newStaticBody(new Transform(Vec3(0), Vec3(1), Mat4(1)), new Collider{AABB {
            Vec3{-walls_x, floor_y - 1, -walls_x},
            Vec3{walls_x, floor_y, walls_x},
        }});

    Model *floor = engine->GetModelManager().LoadModel("Floor_Modular.fbx");
    int floor_cnt = 4;

    float floor_scale = 4.5f;
    for (int i = -floor_cnt; i <= floor_cnt; ++i) {
        for (int j = -floor_cnt; j <= floor_cnt; ++j) {
        Vec3 floor_color;
        if ((i + j) & 1) {
            floor_color = Vec3(0.65f, 0.5f, 0.5f);
        } else {
            floor_color = Vec3(0.5f, 0.5f, 0.65f);
        }
        floor->meshes[0].material.diffuseColor = floor_color;
        newModel(
            new Transform(Vec3(i, 0, j) * 2.0f * floor_scale + Vec3(0, floor_y - floor_scale * 0.125f, 0),
                Vec3(floor_scale), Mat4(1.0)), floor)
            .GetTransform()->Rotate(glm::radians(-90.f), 0, 0);
        }
    }

    Model *ceil = engine->GetModelManager().LoadModel("floor/one_mesh.obj");

    int ceil_cnt = 2;

    float ceil_scale = 5.0f;
    float ceil_y = floor_y + 20.0f;
    for (int i = -ceil_cnt; i <= ceil_cnt; ++i) {
        for (int j = -ceil_cnt; j <= ceil_cnt; ++j) {
        newModel(new Transform(Vec3(i, 0, j) * 3.5f * ceil_scale + Vec3(0, ceil_y + (i & 1) * 0.01f, 0),
                Vec3(ceil_scale), Mat4(1.0)), ceil)
            .GetTransform()->Rotate(glm::radians(180.f), 0, 0);
        }
    }

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            AddLantern(Vec3(i * 25, 12, j * 25));
        }
    }

    float cc_scale = 1.5f;
    Model *cc = engine->GetModelManager().LoadModel("CompanionCube/Portal_Companion_Cube.obj");
    Object cube = newDynamicBody(
        new Transform(Vec3(18, floor_y + cc_scale + 0.1f, 0), Vec3(cc_scale), Mat4(1.0)), cc,
        new Collider{Collider::GetDefaultAABB(&cc->meshes[0])},
        new RigidBody(1.0f, Mat4(0), 0.5f, Vec3(0, -gravity, 0), 1.0f, slidingFriction));
    interactableObjects.push_back(cube);

    float chest_scale = 20.f;
    float chest_y = floor_y + chest_scale * 0.1f;
    Model *chest = engine->GetModelManager().LoadModel("Chest/model.obj");
    for (int i = 0; i < 4; ++i) {
        Object chestObj = newDynamicBody(
            new Transform(Vec3(-20, chest_y, i * 5 - 10), Vec3(chest_scale), Mat4(1.0)),
            chest,
            new Collider{Collider::GetDefaultAABB(&chest->meshes[0])},
            new RigidBody(1.0f, Mat4(0), 0.5f, Vec3(0, -gravity, 0), 1.0f,
                slidingFriction));
        chestObj.GetTransform()->Rotate(0, glm::radians(90.0f), 0);
        interactableObjects.push_back(chestObj);
    }

    auto &modelManager = engine->GetModelManager();
    Model *plant1 = modelManager.LoadModel("HousePlant/Houseplant.obj");
    newModel(new Transform(Vec3(10, floor_y, 0), Vec3(1), Mat4(1.0)), plant1);

    Model *plant2 = modelManager.LoadModel("Orchid/Orchid.obj");
    newModel(new Transform(Vec3(14, floor_y, 0), Vec3(0.3), Mat4(1.0)), plant2);

    Model *dogModel = modelManager.LoadModel("Shiba Inu.glb");
    for (auto& mesh : dogModel->meshes) {
        mesh.material.shininess = 0.8f;
        mesh.material.specularColor = mesh.material.diffuseColor;
        mesh.material.diffuseColor *= 1.5f;
    }


    dogModel->shader = new ShaderProgram(
        engine->GetShaderManager().LoadShaderProgram("skeletal.vshader", "standart.fshader"));


    Object dogObj = engine->NewObject();
    dogSound = engine->NewObject();
    dogSound.AddTransform(Vec3(0), Vec3(0), Mat4(1.f));
    dogSound.AddSound(SoundType::SOUND_3D, "dog-panting-1.mp3", true).SetVolume(10.f).SetRadius(20.f);

    dogObj.AddTransform(Transform(Vec3(14, floor_y, 4), Vec3(1.5f), Mat4(1.0)));
    dogObj.AddSound(SoundType::SOUND_3D, "dog-sniffing.wav", true).SetVolume(10.f).SetRadius(20.f);
    dogObj.AddChild(dogSound);

    dogObj.AddModel(*dogModel);
    class DogBehaviour : public Behaviour {
     private:
        float direction = 0.f;
        float speed = 0.f;
        float phi = 0.04f;
        int prev = -1;

     public:
        void Update(float dt) override {
            // Transform
            self.GetTransform()->Translate(
                Vec3(sin(direction) * speed * dt, 0.f, cos(direction) * speed * dt));
            self.GetTransform()->RotateGlobal(0.f, -phi * speed * dt, 0.f);
            direction -= phi * speed * dt;

            // Animations
            int x = rand();
            if (x % 300 == 0) {
                self.GetSkeletalAnimationsManager()->Stop();
                self.GetSound()->Pause();
                if (prev == 2) {
                    dogSound.GetSound()->Start();
                }
            }

            if (!self.GetSkeletalAnimationsManager()->IsPlaying()) {
                if (x % 3 == 0 && prev != 0) {
                    self.GetSkeletalAnimationsManager()->PlayImmediately(8, 1);
                    speed = 3.2f;
                    prev = 0;
                } else if (x % 3 == 1 && prev != 1) {
                    self.GetSkeletalAnimationsManager()->PlayImmediately(3, 1);
                    speed = 9.f;
                    prev = 1;
                } else {
                    if (prev == 2) {
                        self.GetSkeletalAnimationsManager()->PlayImmediately(8, 1);
                        speed = 3.2f;
                        prev = 0;
                    } else {
                        speed = 0.f;
                        self.GetSkeletalAnimationsManager()->PlayImmediately(2, 1);
                        dogSound.GetSound()->Pause();
                        self.GetSound()->Start();
                        prev = 2;
                    }
                }
            }
        }
    };

    dogObj.AddBehaviour<DogBehaviour>();
    auto& dogAnim = dogObj.AddSkeletalAnimationsManager("Shiba Inu.glb", dogModel, TPoseType::FROMANIM, 11);


    Model *chair = modelManager.LoadModel("Chair.obj");
    for (int i = 0; i < 4; ++i) {
        Transform *chTransform = new Transform(Vec3(5 * i, floor_y, -15), Vec3(0.4), Mat4(1.0));
        Object chairObj = newDynamicBody(chTransform, chair,
            new Collider{Collider::GetDefaultAABB(&chair->meshes[0])},
            new RigidBody(1.0f, Mat4(0), 0.5f, Vec3(0, -gravity, 0), 1.0f, slidingFriction));
        interactableObjects.push_back(chairObj);
    }


    float table_y = 4.2f;

    Model *table = modelManager.LoadModel("Table_Small/Table_Small.obj");
    newStaticBody(new Transform(Vec3(20, floor_y, 14), Vec3(0.05), Mat4(1.0)), table,
        new Collider{Collider::GetDefaultAABB(&table->meshes[0])});

    Model *table2 = modelManager.LoadModel("Desk/desk.obj");
    newStaticBody(new Transform(Vec3(20, floor_y, -30), Vec3(15.0), Mat4(1.0)), table2,
        new Collider{Collider::GetDefaultAABB(&table2->meshes[0])});

    Model *big_shelf = modelManager.LoadModel("wooden bookshelf/model.obj");
    newStaticBody(new Transform(Vec3(20, floor_y + 7, -walls_x + 3), Vec3(8.0), Mat4(1.0)), big_shelf,
        new Collider{Collider::GetDefaultAABB(&big_shelf->meshes[0])}).
        GetTransform()->Rotate(0, glm::radians(-90.0f), 0);

    /*Model *small_shelf = Model::loadFromFile("shelf.obj");
    newStaticBody(new Transform(Vec3(10, floor_y + 10, -walls_x + 3), Vec3(5.0), Mat4(1.0)), small_shelf,
        new Collider{Collider::GetDefaultAABB(&small_shelf->meshes[0])});
    */

    Model *pizza = modelManager.LoadModel("Pizza slice/Pizza_Slice_01.obj");
    Object pizzaObj = newDynamicBody(
        new Transform(Vec3(20, floor_y + table_y, 16), Vec3(0.1), Mat4(1.0)), pizza,
        new Collider{Collider::GetDefaultAABB(&pizza->meshes[0])},
        new RigidBody(1.0f, Mat4(0), 0.5f, Vec3(0, -gravity, 0), 1.0f, slidingFriction));
    pizzaObj.name = 1;
    interactableObjects.push_back(pizzaObj);

    Model *painting = modelManager.LoadModel("Wall painting/Wall_Art_Classical_01.obj");
    newModel(new Transform(Vec3(walls_x, floor_y + 8, 6), Vec3(0.5), Mat4(1.0)), painting)
        .GetTransform()->Rotate(0, glm::radians(90.0f), 0);

    Model *boomboxModel = modelManager.LoadModel("record player.fbx");
    auto boombox = newModel(new Transform(Vec3(20, floor_y + table_y, 14),
        Vec3(1.0, 1.0, 0.25), Mat4(1.0)), boomboxModel);
    boombox.GetTransform()->Rotate(glm::radians(-90.0f), 0, 0);
}

void poolTable() {
    float balls_y = -2.0f;
    std::vector<Vec3> coordinates {
        Vec3(-2, balls_y, -0.2f),
        Vec3(-2.5f, balls_y, -0.5f),
        Vec3(-2.5f, balls_y, 0.1f),
        Vec3(-3.f, balls_y, -0.8f),
        Vec3(-3.f, balls_y, -0.2f),
    /*    Vec3(-3.f, balls_y, 0.4f),
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
    std::vector<Object> balls;
    for (int i = 0; i < ballsCount; i++) {
        Vec3 pos = coordinates[i];
        auto newBall = MovingBall::New(pos, 0.2f, "pool/" + std::to_string(i % 16 + 1) + ".png");
        balls.push_back(newBall);
    }

    auto ocraFont = engine->GetFontManager().LoadFont("OCRAEXT.TTF", 20);
    auto scoreText = PublicText::New(ocraFont, "0", Vec2(0.02f, 0.95f), 1.f, Vec3(0, 0, 0));
    Object gmObj = engine->NewObject();
    gmObj.AddBehaviour<GameManager>(
        reinterpret_cast<PublicText*>(scoreText.GetBehaviour()));
    GameManager *gameManager = reinterpret_cast<GameManager*>(gmObj.GetBehaviour());

    float table_y = -7;

    Table::New(Vec3(0, table_y, 0), Vec3(5), gameManager);

    Cue::New(balls, engine->camera);
}

int main() {
    init();

    buildRoom();
    poolTable();
    createUI();

    auto ocraFont = engine->GetFontManager().LoadFont("OCRAEXT.TTF", 20);
    auto hintText = reinterpret_cast<PublicText*>(
        PublicText::New(ocraFont, "0", Vec2(0.53f, 0.5f), 1.f, Vec3(1))
        .GetBehaviour());
    Object player = engine->NewObject();
    player.AddBehaviour<PlayerController>(engine->camera, interactableObjects, hintText);
    player.AddTransform(Vec3(0.f, 1.5f, 14.f), Vec3(1), Mat4(1));
    player.AddCollider(AABB{Vec3(-0.6, -7, -0.6f), Vec3(0.6f, 1, 0.6f)});
    float player_mass = 5.0f;
    player.AddRigidBody(player_mass, Mat4(0),
                0.5f, Vec3(0, -gravity * player_mass, 0), 0.f, slidingFriction);
    engine->Run();
}
