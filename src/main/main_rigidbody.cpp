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

const char *standartVertexShaderSource = "standart.vshader";
const char *skeletalVertexShaderSource = "skeletal.vshader";
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
            self.GetTransform()->Rotate(0.01f, Vec3(1.f));
        }
    }
};

class MovingRotating2 : public Behaviour {
 public:
    Vec3 speed;
    void Update(float dt) override {
        if (self.CollideAll().size() == 0) {
            self.GetTransform()->Translate(Vec3(2.f, 0.f, 0.f) * dt);
            self.GetTransform()->Rotate(0.05f, Vec3(1.f, 0.f, 1.f));
        }
    }
};

int main() {
    auto engine = Engine();

    Shader standartVShader = Shader(VertexShader, standartVertexShaderSource);
    Shader skeletalVShader = Shader(VertexShader, skeletalVertexShaderSource);
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);

    ShaderProgram *standartShaderProgram = new ShaderProgram(standartVShader, fShader);
    ShaderProgram *skeletalShaderProgram = new ShaderProgram(skeletalVShader, fShader);

    {
        // /* PIGEON */
        Model *pigeonModel = Model::loadFromFile("pigeon/scene.gltf");
        pigeonModel->shader = skeletalShaderProgram;
        auto pigeonAnimation = new SkeletalAnimationData("pigeon/scene.gltf", 0, pigeonModel);

        auto pigeonObj = engine.NewObject();
        pigeonObj.AddTransform(Vec3(0.f, -10.f, -10.f), Vec3(10.f), Mat4(1.0));
        pigeonObj.AddModel(*pigeonModel);
        pigeonObj.AddSkeletalAnimationsManager(pigeonAnimation).PlayImmediately(0, 0);
    }

    {
        /* Wolf */
        Model *wolfModel = Model::loadFromFile("Wolf/Wolf-Blender-2.82a.gltf", skeletalShaderProgram);
        wolfModel->meshes.pop_back();  // Delete Fur
        wolfModel->meshes.erase(wolfModel->meshes.begin() + 1);  // Delete floor

        auto wolfObj = engine.NewObject();
        wolfObj.AddTransform(Vec3(5.f, -10.f, -10.f), Vec3(10.f), Mat4(1.0));
        wolfObj.AddModel(*wolfModel);
        wolfObj.AddSkeletalAnimationsManager("Wolf/Wolf-Blender-2.82a.gltf", wolfModel).PlayImmediately(3, 1);

        class WolfBehaviour : public Behaviour {
         public:
            void Update(float dt) override {
                left -= dt;
                if (left < 0) {
                    self.GetSkeletalAnimationsManager()->Stop();
                }
                if (!self.GetSkeletalAnimationsManager()->IsPlaying()) {
                    left = delay;
                    self.GetSkeletalAnimationsManager()->PlayImmediately(cur, 1);
                    cur++;
                    cur %= 5;
                }
            }

            float delay = 4.0f;
            float left = delay;
            int cur = 0;
        };

       wolfObj.AddBehaviour<WolfBehaviour>();
    }

    // {
    //     /* XBot */
    //     Model *pigeonModel = Model::loadFromFile("XBot/XBot.dae", skeletalShaderProgram);

    //     auto pigeonAnimation1 = new SkeletalAnimationData("XBot/Praying.dae", 0, pigeonModel);

    //     auto pigeonObj = engine.NewObject();
    //     pigeonObj.AddTransform(Vec3(-6.f, -10.f, -10.f), Vec3(5.f), Mat4(1.0));
    //     pigeonObj.AddModel(*pigeonModel);
    //     auto& animManager = pigeonObj.AddSkeletalAnimationsManager(pigeonAnimation1);
    //     animManager.AddAnimation("XBot/Hip Hop Dancing.dae", pigeonModel);
    //     Logger::Info("%s", animManager.GetAnimationsInfo().c_str());
    //     animManager.PlayImmediately(1, 1);
    // }


    {
        Model * model = Model::loadFromFile(catSource);
        model->shader = standartShaderProgram;
        Material cat_material = {
            4.f,
            Texture("/Cat_diffuse.png", "/Cat_specular.png")
        };
        auto cat = engine.NewObject();
        cat.AddModel(*model);
        auto &t = cat.AddTransform(Vec3(0.f, -7.f, -5.f), Vec3(0.01f), Mat4(1.0));
        t.RotateGlobal(1.67f, Vec3(-1.f, 0.f, 0.f));
    }

    // Shiba inu (ETO FIASKO BRATAN)
    Model *model = Model::loadFromFile("ShibaInu.fbx");
    model->shader = standartShaderProgram;
    auto dog = engine.NewObject();
    dog.AddModel(*model);
    // dog.AddSkeletalAnimationsManager("ShibaInu.fbx", model).PlayImmediately(14, 1);
    dog.AddTransform(Transform(Vec3(2, -5, 0.0), Vec3(1.f), glm::radians(-90.f), Vec3(1.0f, 0.f, 0.f)));

    Material material = {
        4.f,
        Texture("wall.png", "wallspecular.png")
    };
    Model *sphereModel = Model::fromMesh(Mesh::GetSphere(), material, standartShaderProgram);
    Model *cubeModel = Model::fromMesh(Mesh::GetCube(), material, standartShaderProgram);

    auto setUpObj = [=, &engine](Transform transform, auto primitive, Model *model,
            Vec3 speed, float mass, Vec3 angUnlock) {
        auto obj = engine.NewObject();
        obj.AddModel(*model);

        obj.AddTransform(transform);
        obj.AddCollider(primitive);
        obj.AddRigidBody(RigidBody(mass, IBodyOBB(Vec3(1), mass), speed, 0,
                    Vec3(0, -mass * 10, 0), angUnlock, 0.05));
        return obj;
    };

    auto cat = engine.NewObject();
    cat.AddModel(*model);
    auto &t = cat.AddTransform(Vec3(0.f, -5.f, -8.f), Vec3(0.1f), Mat4(1.0));
    // cat.AddCollider(&model->meshes[0]);


    auto staticAABB = setUpObj(
        Transform(Vec3(2, -30, 2.0), Vec3(50), 0.f, Vec3(1)),
        OBB {
            Vec3(0, 0, 0),
            Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)),
            Vec3(0.5, 0.5, 0.5),
        },
        cubeModel,
        Vec3(0, 0, 0),
        0,
        Vec3(0));


    auto obb = setUpObj(
        Transform(Vec3(2, 3, 2.0), Vec3(1), 45, Vec3(1, 0, 0)),
        OBB {
            Vec3(0, 0, 0),
            Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)),
            Vec3(0.5, 0.5, 0.5),
        },
        cubeModel,
        Vec3(0, 0, 0),
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
        Vec3(3, 0, 0),
        1,
        Vec3(0));

    auto aabb3 = setUpObj(
        Transform(Vec3(0, 0, 2.0), Vec3(1), 0.f, Vec3(1)),
        AABB {
            Vec3(-0.5),
            Vec3(0.5)
        },
        cubeModel,
        Vec3(0, 0, 0),
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
            speed, 0, Vec3(0, -mass * 10, 0), Vec3(1), 0.01));
        return obj;
    };

    /*
    Object spheres[3] = {
        getSphereObj(
            Transform(Vec3(-10, 4, 2.0), Vec3(1), 0.f, Vec3(1)),
            Vec3(1, 0, 0),
            1),
        getSphereObj(
            Transform(Vec3(5, 4, 2.0), Vec3(1), 0.f, Vec3(1)),
            Vec3(0, 0, 0),
            4.f),
        getSphereObj(
            Transform(Vec3(10, 4, 2.0), Vec3(1.0), 0, Vec3(1)),
            Vec3(0, 0, 0),
            10000)
    };
    */

    // cat.AddChild(aabb);
    // cat.AddBehaviour<Moving>();

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
