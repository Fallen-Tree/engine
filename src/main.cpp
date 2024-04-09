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

class CameraController : public Behaviour {
 private:
    enum MovementMode {Walk, Fly};
    Camera * m_Camera;
    float m_FlySpeed = 8;
    float m_WalkSpeed = 15;
    float m_MouseSensitivity = SENSIVITY;
    MovementMode m_MovementMode = Fly;

    void ProcessKeyboard(float deltaTime) {
        if (s_Input->IsKeyPressed(Key::Space)) {
            if (m_MovementMode == Fly) {
                m_MovementMode = Walk;
            } else {
                m_MovementMode = Fly;
            }
        }

        Vec3 front = m_Camera->GetFront();
        if (m_MovementMode == Walk) {
            front.y = 0;
        }
        front = glm::normalize(front);
        Vec3 right = glm::normalize(glm::cross(front, Vec3(0, 1, 0)));

        Vec3 direction = Vec3(0);

        if (s_Input->IsKeyDown(Key::W))
            direction += front;
        if (s_Input->IsKeyDown(Key::S))
            direction -= front;
        if (s_Input->IsKeyDown(Key::A))
            direction -= right;
        if (s_Input->IsKeyDown(Key::D))
            direction += right;
        float moveSpeed;
        if (m_MovementMode == Walk)
            moveSpeed = m_WalkSpeed;
        else /*m_MovementMode == Fly*/
            moveSpeed = m_FlySpeed;
        Transform *tr = self.GetTransform();
        tr->Translate(direction * moveSpeed * deltaTime);
    }

    void ProcessMouseMovement() {
        float xOffset = s_Input->OffsetX();
        float yOffset = s_Input->OffsetY();

        xOffset *= m_MouseSensitivity;
        yOffset *= m_MouseSensitivity;
 
        Transform *tr = self.GetTransform();

        tr->Rotate(0, xOffset, 0);
        tr->RotateGlobal(-yOffset, 0, 0);

        // to update front vector
        m_Camera->SetTransform(*self.GetTransform());
 
        Vec3 front = m_Camera->GetFront();
        if (abs(glm::dot(front, Vec3(0, 1, 0))) > MAX_DOT) {
            tr->RotateGlobal(yOffset, 0, 0);
        }
    }

    void ProcessMouseScroll() {
        float scrollOffset = s_Input->ScrollOfsset();
        float zoom = m_Camera->GetZoom();
        zoom -= static_cast<float>(scrollOffset);
        if (zoom < MIN_FOV) zoom = MIN_FOV;
        if (zoom > MAX_FOV) zoom = MAX_FOV;
        m_Camera->SetZoom(zoom);
    }

 public:
    void Update(float deltaTime) {
        if (m_Camera == nullptr) return;

        ProcessMouseMovement();

        ProcessMouseScroll();

        ProcessKeyboard(deltaTime);

        m_Camera->SetTransform(*self.GetTransform());
    }

    explicit CameraController(Camera * camera) {
        m_Camera = camera;
    }
};

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

    auto setUpObj = [=, &engine](Transform transform, auto primitive, Model *model) {
        auto obj = engine.NewObject();
        obj.AddModel(*model);

        obj.AddTransform(transform);
        obj.AddCollider(primitive);
        obj.AddRigidBody(0.f, Mat4(0), Vec3(0), 1.f, Vec3(0), Vec3(0), 0.01f);
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
           speed, 0.1f, Vec3(0, -mass * 10, 0), Vec3(1), 0.01f);
        obj.AddBehaviour<MovingSphere>();
        return obj;
    };

    Object spheres[3] = {
        getSphereObj(
            Transform(Vec3(-30, -1, 2.0), Vec3(1), 0.f, Vec3(1)),
            Vec3(10, 0, 0),
            4),
        getSphereObj(
            Transform(Vec3(0, -1, 2.0), Vec3(1), 0.f, Vec3(1)),
            Vec3(0, 0, 0),
            4.f),
        getSphereObj(
            Transform(Vec3(10, -1, 2.0), Vec3(1.0), 0, Vec3(1)),
            Vec3(0, 0, 0),
            1000),
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

    Object control = engine.NewObject();
    control.AddBehaviour<CameraController>(engine.camera);
    control.AddTransform(STARTING_POSITION, Vec3(1), Mat4(1));
    engine.Run();
}
