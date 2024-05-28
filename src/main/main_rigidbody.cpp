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
    auto engine = new Engine("test");

    ShaderProgram skeletalProgram = engine
        ->GetShaderManager()
        .LoadShaderProgram(skeletalVertexShaderSource, fragmentShaderSource);

    Material material = {
        4.f,
        Texture("wall.png", "wallspecular.png")
    };
    Model *sphereModel = Model::fromMesh(Mesh::GetSphere(), material);
    Model *cubeModel = Model::fromMesh(Mesh::GetCube(), material);

    auto setUpObj = [=, &engine](Transform transform, auto primitive, Model *model,
            Vec3 speed, Vec3 angularSpeed, float mass, Vec3 angUnlock) {
        auto obj = engine->NewObject();
        obj.AddModel(*model);

        obj.AddTransform(transform);
        obj.AddCollider(primitive);
        obj.AddRigidBody(mass, IBodyOBB(Vec3(1), mass), speed,
                    angularSpeed, 0.f, Vec3(0, -mass * 10, 0), angUnlock, 0.05f,
                    slidingFriction);
        return obj;
    };

    auto staticAABB = setUpObj(
        Transform(Vec3(2, -30, 2.0), Vec3(50), Mat4(1)),
        AABB {
            Vec3(-0.5),
            Vec3(0.5),
        },
        cubeModel,
        Vec3(0, 0, 0),
        Vec3(0),
        0,
        Vec3(0));


    auto obb = setUpObj(
        Transform(Vec3(2, 0, 2.0), Vec3(1), 20, Vec3(1)),
        OBB {
            Vec3(0, 0, 0),
            Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)),
            Vec3(0.5, 0.5, 0.5),
        },
        cubeModel,
        Vec3(0, 0, 0),
        Vec3(0.0, 0, 0),
        1,
        Vec3(1));

    auto obb2 = setUpObj(
        Transform(Vec3(-10, 0, 2.0), Vec3(1), 10, Vec3(1)),
        OBB {
            Vec3(0, 0, 0),
            Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)),
            Vec3(0.5, 0.5, 0.5),
        },
        cubeModel,
        Vec3(5, 0, 0),
        Vec3(0.0, 0, 0),
        1,
        Vec3(1));

    auto getSphereObj = [=, &engine](Transform transform, Vec3 speed, float mass) {
        auto obj = engine->NewObject();
        obj.AddTransform(transform);
        obj.AddModel(*sphereModel);
        obj.AddCollider(Sphere{ Vec3(0), 1.f });
        obj.AddRigidBody(RigidBody(
            mass, IBodySphere(transform.GetScale().x, mass),
            speed, Vec3(0), 0, Vec3(0, -mass * 10, 0), Vec3(1), 0.01, rollingFriction));
        obj.GetRigidBody()->typeFriction = rollingFriction;
        return obj;
    };

    getSphereObj(
        Transform(Vec3(-10, 4, 2.0), Vec3(1), 0.f, Vec3(1)),
        Vec3(5, 0, 0),
        1);
    getSphereObj(
        Transform(Vec3(5, 4, 2.0), Vec3(1), 0.f, Vec3(1)),
        Vec3(0, 0, 0),
        4.f);
    getSphereObj(
        Transform(Vec3(10, 4, 2.0), Vec3(1.0), 0, Vec3(1)),
        Vec3(0, 0, 0),
        10000);

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
        auto ocraFont = engine->GetFontManager().LoadFont("OCRAEXT.TTF", 20);
        auto obj = engine->NewObject();
        obj.AddText(ocraFont, "", 0.85f, 0.95f, 1.f, Vec3(0, 0, 0));
        obj.AddBehaviour<FpsText>();
    }

    engine->NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(-0.2, -0.5, -1.2),
        1.f, 0.09f, 0.032f);

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

    engine->NewObject().AddSpotLight(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f),
        1.0f, 0.09f, 0.032f, Vec3(0),
        glm::cos(glm::radians(12.5f)),
        glm::cos(glm::radians(15.0f)));

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

    Object control = engine->NewObject();
    control.AddBehaviour<CameraController>(engine->camera);
    control.AddTransform(STARTING_POSITION, Vec3(1), Mat4(1));

    engine->Run();
}
