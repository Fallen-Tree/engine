#include <irrKlang.h>

#include <limits>

#include "engine.hpp"
#include "collisions.hpp"
#include "logger.hpp"
#include "sound.hpp"
#include <glm/gtx/string_cast.hpp>

const char *cubeSource = "/cube2.obj";
const char *catSource = "cat.obj";
const char *benchSource = "/bench.obj";

const char *vertexShaderSource = "standart.vshader";
const char *fragmentShaderSource = "standart.fshader";

class MovingSphere : public Object {
 public:
    void Update(float dt) override {
    }
};


extern irrklang::ISoundEngine *SoundEngine;

int main() {
    auto engine = Engine();

    std::vector<GLfloat> cubeVertices {
          // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    Shader vShader = Shader(VertexShader, vertexShaderSource);
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);
    ShaderProgram *shaderProgram = new ShaderProgram(vShader, fShader);

    // ShaderProgram shaderProgram = ShaderProgram(vShader, fShader2);
    // init a model
    Model *cubeModel = new Model(cubeVertices, 8);
    cubeModel->shader = shaderProgram;
    Model *sphereModel = Model::GetSphere();
    sphereModel->shader = shaderProgram;
    Model * model = Model::loadFromFile(catSource);

    auto obj = new Object();
    model->shader = shaderProgram;
    obj->renderData = new RenderData(model,
        {4.f, Texture("/Cat_diffuse.png", "/Cat_specular.png")});

    obj->collider = new Collider{Collider::GetDefaultAABB(model)};
    obj->rigidbody = new RigidBody(100, Mat3(0), Vec3(0), 0, Vec3(0, -1000, 0),
        Vec3(0), Vec3(1), 0.1);

    Material material = {
        4.f,
        Texture("/wall.png", "/wallspecular.png")
    };

    auto setUpObj = [=, &engine](Transform transform, auto primitive, Model *model) {
        auto obj = new Object();
        model->shader = shaderProgram;
        obj->renderData = new RenderData(model, material);

        obj->transform = new Transform(transform);
        obj->collider = new Collider { primitive };
        obj->rigidbody = new RigidBody(0, Mat4(0), Vec3(0), 3, Vec3(0), Vec3(0),
            Vec3(0), 0.0001);
        engine.AddObject<>(obj);
        return obj;
    };

    auto aabb = setUpObj(
        Transform(Vec3(0, -31, 0), Vec3(50), 0, Vec3(1)),
        AABB {
            Vec3{-0.5, -0.5, -0.5},
            Vec3{0.5, 0.5, 0.5},
        },
        cubeModel);

    auto getSphereObj = [=](Transform transform, Vec3 speed, float mass) {
        auto obj = new MovingSphere();
        obj->renderData = new RenderData(sphereModel, material);
        obj->renderData->model->shader = shaderProgram;

        obj->transform = new Transform(transform);

        obj->collider = new Collider{Sphere{
            Vec3(0),
            1,
        }};
        obj->rigidbody = new RigidBody(mass, IBodySphere(1, 20),
                speed, 0, Vec3(0, -mass * 10, 0), Vec3(1), Vec3(1), 0.0005);
        return obj;
    };

    Object *spheres[3] = {
        getSphereObj(
            Transform(Vec3(-2, 100, 2.0), Vec3(1), 0, Vec3(1)),
            Vec3(1, 0, 0),
            2),
        getSphereObj(
            Transform(Vec3(0, 100, 2.0), Vec3(1), 0, Vec3(1)),
            Vec3(0, 0, 0),
            1),
        getSphereObj(
            Transform(Vec3(4, 120, 2.0), Vec3(1.0), 0, Vec3(1)),
            Vec3(0, -100, 0),
            3),
    };
    engine.AddObject<>(spheres[0]);
    engine.AddObject<>(spheres[1]);
    engine.AddObject<>(spheres[2]);

    class FpsText : public Object {
     public:
        void Update(float dt) override {
            int fps = Time::GetCurrentFps();
            char buf[12];
            snprintf(buf, sizeof(buf), "Fps: %d", fps);
            this->text->SetContent(buf);
        }
    };

    auto textOcra = new Font("OCRAEXT.TTF", 20);
    auto fpsObj = new FpsText();
    fpsObj->text = new Text(textOcra, "", 0.85, 0.96, 1.0, Vec3(0, 0, 0));
    engine.AddObject<>(fpsObj);


    // Sphere just for updating movement
    auto musicObject1 = new Object();
    musicObject1->sound = new Sound(SOUND_FLAT, "georgian_disco.mp3");
    musicObject1->sound->SetVolume(0.2f);
    musicObject1->sound->Start();
    engine.AddObject<>(musicObject1);

    // Sphere just for updating movement
    Object* musicObject2 = getSphereObj(
            Transform(Vec3(-4, 100, 2.0), Vec3(1.0), 0, Vec3(1)),
            Vec3(0, -100, 0),
            3);
    musicObject2->sound = new Sound(SOUND_3D, "explosion.wav");
    musicObject2->sound->SetRadius(20.f);
    musicObject2->sound->Loop();
    musicObject2->sound->Start();

    engine.AddObject<>(musicObject2);


    Object* healthBar1 = new Object();
    healthBar1->image = new Image("hp.png", 0.03, 0.15, 0.4);
    engine.AddObject<>(healthBar1);

    Object* healthBar2 = new Object();
    healthBar2->image = new Image("hp_bar.png", 0.015, 0.01, 0.4);
    engine.AddObject<>(healthBar2);

    // init light objects
    Object* pointLight1 = new Object();
    pointLight1->light = new PointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(-0.2, -0.5, -1.2),
        1, 0.09f, 0.032f);
    auto pointLight = std::get<PointLight*>(pointLight1->light);
    engine.AddObject<>(pointLight1);

    Object* pointLight2 = new Object();
    pointLight2->light = new PointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(2.3f, -3.3f, -4.0f),
        1, 0.09f, 0.032f);
    pointLight = std::get<PointLight*>(pointLight2->light);
    engine.AddObject<>(pointLight2);

    Object* pointLight3 = new Object();
    pointLight3->light = new PointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f,  0.0f, -3.0f),
        1, 0.09f, 0.032f);
    pointLight = std::get<PointLight*>(pointLight3->light);
    engine.AddObject<>(pointLight3);

    Object* dirLight = new Object();
    dirLight->light = new DirLight(
            Vec3(0.05f, 0.05f, 0.05f), Vec3(0.4f, 0.4f, 0.4f),
            Vec3(0.5f, 0.5f, 0.5f),  Vec3(-0.2f, -1.0f, -0.3f));

    auto directionLight = std::get<DirLight*>(dirLight->light);
    engine.AddObject<>(dirLight);

    Object* spotLight = new Object();
    spotLight->light = new SpotLight(
            Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
            Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f),
            1.0f, 0.09f, 0.032f, Vec3(0),
            glm::cos(glm::radians(12.5f)),
            glm::cos(glm::radians(15.0f)));
    auto sptLight = std::get<SpotLight*>(spotLight->light);
    engine.AddObject<>(spotLight);

    engine.Run();
}
