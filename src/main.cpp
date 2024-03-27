#include <limits>
#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "collisions.hpp"
#include "logger.hpp"

const char *cubeSource = "cube2.obj";
const char *catSource = "cat.obj";
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
        self.GetTransform()->Translate(Vec3 {2, 0, 0} * dt);
        if (s_Input->IsKeyPressed(Key::Space)) {
            self.Remove();
        }
    }
};

int main() {
    auto engine = Engine();

    Shader vShader = Shader(VertexShader, vertexShaderSource);
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);

    ShaderProgram *shaderProgram = new ShaderProgram(vShader, fShader);

    // init a model
    Model * model = Model::loadFromFile(catSource);
    model->shader = shaderProgram;
    Material cat_material = {
        4.f,
        Texture("/Cat_diffuse.png", "/Cat_specular.png")
    };
    model->setMaterial(cat_material);
    /*
    auto cat = engine.NewObject();
    cat.AddModel(*model);
    auto &t = cat.AddTransform(Vec3(0.f, -10.f, -8.f), Vec3(1.f), Mat4(1.0));
    t.Rotate(1.67f, Vec3(-1.f, 0.f, 0.f));
    cat.AddCollider(Collider::GetDefaultAABB(&model->meshes[0]));
    cat.AddRigidBody(100.f, Mat3(0), Vec3(0), 0.f, Vec3(0, 0, 0),
        Vec3(0), Vec3(0), 0.1f);
        */

    Material material = {
        4.f,
        Texture("/wall.png", "/wallspecular.png")
    };
    Model *sphereModel = Model::fromMesh(Mesh::GetSphere(), material);
    Model *cubeModel = Model::fromMesh(Mesh::GetCube(), material);
    sphereModel->shader = shaderProgram;
    cubeModel->shader = shaderProgram;

    auto setUpObj = [=, &engine](Transform transform, auto primitive, Model *model) {
        auto obj = engine.NewObject();
        obj.AddModel(*model);

        obj.AddTransform(transform);
        obj.AddCollider(primitive);
        obj.AddRigidBody(0.0f, Mat4(0), Vec3(0), 3.0f, Vec3(0), Vec3(0), Vec3(0), 0.01f);
        return obj;
    };


    auto aabb = setUpObj(
        Transform(Vec3(-15, -55, 0), Vec3(100), 0.0f, Vec3(1)),
        AABB {
            Vec3{-0.5, -0.5, -0.5},
            Vec3{0.5, 0.5, 0.5},
        },
        cubeModel);

    auto getSphereObj = [=, &engine](Transform transform, Vec3 speed, float mass) {
        auto obj = engine.NewObject();
        obj.AddTransform(transform);
        obj.AddModel(*sphereModel);
        obj.AddCollider(Sphere{ Vec3(0), 1.f });
        obj.AddRigidBody(mass, IBodySphere(1, mass),
                speed, 1.f, Vec3(0, -mass * 10, 0), Vec3(1), Vec3(1), 0.03f);
        obj.AddBehaviour<MovingSphere>();
        return obj;
    };

    Object spheres[3] = {
        getSphereObj(
            Transform(Vec3(-20, 2, 2.0), Vec3(1), 0.f, Vec3(1)),
            Vec3(8, 0, 0),
            40),
        getSphereObj(
            Transform(Vec3(0, 2, 2.0), Vec3(1), 0.f, Vec3(1)),
            Vec3(0, 0, 0),
            3.f),
        getSphereObj(
            Transform(Vec3(10, 2, 2.0), Vec3(1.0), 0, Vec3(1)),
            Vec3(0, 0, 0),
            1000),
    };
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
        obj.AddText(ocraFont, "", 685.0f, 575.0f, 1.f, Vec3(0, 0, 0));
        obj.AddBehaviour<FpsText>();
        /*
        obj.AddBehaviour<>([](auto &self, float dt) {
            int fps = Time::GetCurrentFps();
            char buf[12];
            snprintf(buf, sizeof(buf), "Fps: %d", fps);
            self.GetText()->SetContent(buf);
        });
         */
    }

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
