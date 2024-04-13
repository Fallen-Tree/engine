#include <limits>

#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "collisions.hpp"
#include "logger.hpp"
#include "sound.hpp"
#include <glm/gtx/string_cast.hpp>

const char *cubeSource = "cube2.obj";
const char *catSource = "fish.obj";
const char *benchSource = "bench.obj";

const char *vertexShaderSource = "standart.vshader";
const char *fragmentShaderSource = "standart.fshader";

class MovingSphere : public Behaviour {
 public:
    void Update(float dt) override {
    }
};

class Moving : public Behaviour {
 public:
    void Update(float dt) override {
        if (self.CollideAll().size() == 0)
            self.GetTransform()->Translate(Vec3(0, -0.5, 0) * dt);
    }
};

class MovingRotating : public Behaviour {
 public:
    void Update(float dt) override {
        if (self.CollideAll().size() == 0) {
            self.GetTransform()->Translate(Vec3(-1, -1, -1) * dt);
            self.GetTransform()->Rotate(0.01, Vec3(1));
        }
    }
};

class MovingRotating2 : public Behaviour {
 public:
    Vec3 speed;
    void Update(float dt) override {
        if (self.CollideAll().size() == 0) {
            self.GetTransform()->Translate(Vec3(2, 0, 0) * dt);
            self.GetTransform()->Rotate(0.05, Vec3(1, 0, 1));
        }
    }
};

int main() {
    auto engine = Engine();

    Shader vShader = Shader(VertexShader, vertexShaderSource);
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);

    ShaderProgram *shaderProgram = new ShaderProgram(vShader, fShader);

    // Shiba inu (ETO FIASKO BRATAN)
    Model *model = Model::loadFromFile("ShibaInu.fbx");
    model->shader = shaderProgram;
    auto dog = engine.NewObject();
    dog.AddModel(*model);
    dog.AddTransform(Transform(Vec3(2, -5, 0.0), Vec3(1.f), glm::radians(-90.f), Vec3(1.0f, 0.f, 0.f)));

    Material material = {
        4.f,
        Texture("wall.png", "wallspecular.png")
    };
    Model *sphereModel = Model::fromMesh(Mesh::GetSphere(), material);
    Model *cubeModel = Model::fromMesh(Mesh::GetCube(), material);
    sphereModel->shader = shaderProgram;
    cubeModel->shader = shaderProgram;

    auto setUpObj = [=, &engine](Transform transform, auto primitive, Model *model,
            Vec3 speed, float mass, Vec3 angUnlock) {
        auto obj = engine.NewObject();
        obj.AddModel(*model);

        obj.AddTransform(transform);
        obj.AddCollider(primitive);
        obj.AddRigidBody(RigidBody(mass, IBodyOBB(Vec3(1), mass), speed, 0.3,
                    Vec3(0, -mass * 10, 0), angUnlock, 0.2));
        return obj;
    };

    auto cat = engine.NewObject();
    cat.AddModel(*model);
    auto &t = cat.AddTransform(Vec3(0.f, -5.f, -8.f), Vec3(0.1f), Mat4(1.0));
    //cat.AddCollider(&model->meshes[0]);


    auto aabb = setUpObj(
        Transform(Vec3(2, -30, 2.0), Vec3(50), 0.f, Vec3(1)),
        AABB {
            Vec3(-0.5),
            Vec3(0.5)
        },
        cubeModel,
        Vec3(0, 0, 0),
        0,
        Vec3(0));

    auto obb = setUpObj(
        Transform(Vec3(2, 0, 2.0), Vec3(1), 45, Vec3(1, 0, 1)),
        OBB {
            Vec3(0, 0, 0),
            Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)),
            Vec3(0.5, 0.5, 0.5),
        },
        cubeModel,
        Vec3(3, 0, 0),
        1,
        Vec3(1));

    /*
    auto aabb2 = setUpObj(
        Transform(Vec3(-4, 0, 2.0), Vec3(1), 0.f, Vec3(1)),
        AABB {
            Vec3(-0.5),
            Vec3(0.5)
        },
        cubeModel,
        Vec3(1, 0, 0),
        1,
        Vec3(0));

    auto obb2 = setUpObj(
        Transform(Vec3(0, -1, 2.0), Vec3(2), 0.0f, Vec3(1)),
        OBB {
            Vec3(0, 0, 0),
            Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)),
            Vec3(0.5, 0.5, 0.5),
        },
        cubeModel,
        Vec3(0),
        1);


    auto obb3 = setUpObj(
        Transform(Vec3(10, 13, 10.0), Vec3(2), 0.0f, Vec3(1)),
        OBB {
            Vec3(0, 0, 0),
            Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)),
            Vec3(0.5, 0.5, 0.5),
        },
        cubeModel,
        Vec3(0),
        1);
        */

    auto getSphereObj = [=, &engine](Transform transform, Vec3 speed, float mass) {
        auto obj = engine.NewObject();
        obj.AddTransform(transform);
        obj.AddModel(*sphereModel);
        obj.AddCollider(Sphere{ Vec3(0), 1.f });
        obj.AddRigidBody(RigidBody(
            mass, IBodySphere(transform.GetScale().x, mass),
            speed, 0, Vec3(0, -mass, 0), Vec3(1), 0.1));
        return obj;
    };

    Object spheres[3] = {
        getSphereObj(
            Transform(Vec3(-10, 0, 2.0), Vec3(1), 0.f, Vec3(1)),
            Vec3(3, 0, 0),
            10),
        getSphereObj(
            Transform(Vec3(5, 0, 2.0), Vec3(1), 0.f, Vec3(1)),
            Vec3(0, 0, 0),
            4.f),
        getSphereObj(
            Transform(Vec3(10, 0, 2.0), Vec3(1.0), 0, Vec3(1)),
            Vec3(0, 0, 0),
            10000)
    };

    class FpsText : public Behaviour {
     public:
        void Update(float dt) override {
            int fps = Time::GetCurrentFps();
            char buf[12];
            snprintf(buf, sizeof(buf), "Fps: %d", fps);
            self.GetText()->SetContent(buf);
        }
    };

    {
        auto ocraFont = new Font("OCRAEXT.TTF", 20);
        auto obj = engine.NewObject();
        obj.AddText(ocraFont, "", 0.85f, 0.95f, 1.f, Vec3(0, 0, 0));
        obj.AddBehaviour<FpsText>();
    }

    engine.NewObject().AddSound(SoundType::SOUND_FLAT, "georgian_disco.mp3").SetVolume(0.05f).Start();

    engine.NewObject().AddImage("hp.png", 0.03f, 0.15f, 0.4f);
    engine.NewObject().AddImage("hp_bar.png", 0.015f, 0.01f, 0.4f);

    engine.NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(-0.2, -0.5, -1.2),
        1.f, 0.09f, 0.032f);

    engine.NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(2.3f, -3.3f, -4.0f),
        1.f, 0.09f, 0.032f);

    engine.NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f,  0.0f, -3.0f),
        1.f, 0.09f, 0.032f);

    engine.NewObject().AddDirLight(
        Vec3(0.05f, 0.05f, 0.05f), Vec3(0.4f, 0.4f, 0.4f),
        Vec3(0.5f, 0.5f, 0.5f),  Vec3(-0.2f, -1.0f, -0.3f));

    engine.NewObject().AddSpotLight(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f),
        1.0f, 0.09f, 0.032f, Vec3(0),
        glm::cos(glm::radians(12.5f)),
        glm::cos(glm::radians(15.0f)));

    engine.Run();
}
