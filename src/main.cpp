#include <limits>
#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "collisions.hpp"
#include "logger.hpp"
#include "render_data.hpp"

const char *cubeSource = "/cube2.obj";
const char *catSource = "/cat.obj";
const char *benchSource = "/bench.obj";

const char *vertexShaderSource = "/standart.vshader";
const char *fragmentShaderSource = "/standart.fshader";

class MovingSphere : public Behaviour {
 public:
    void Update(float dt) override {
        if (!m_Target.IsValid()) return;
        if (!self.GetCollider()->Collide(*self.GetTransform(), m_Target.GetCollider(), *m_Target.GetTransform())) {
            self.GetTransform()->Translate(m_Speed * dt);
        } else {
            m_Target.Remove();
        }
    }

    MovingSphere(Object target, Vec3 speed) : m_Target(target) {
        m_Speed = speed;
    }
 private:
    Object m_Target;
    Vec3 m_Speed;
};

class Pointer : public Behaviour {
 public:
    Pointer(std::vector<Object> objects, Camera *camera) {
        m_Camera = camera;
        m_Objects = objects;
    }

    void Update(float dt) override {
        Ray ray = m_Camera->GetRayThroughScreenPoint(m_Camera->GetScreenSize() / 2.0f);
        Transform *target = nullptr;
        float closest = std::numeric_limits<float>::max();
        for (int i = 0; i < m_Objects.size(); i++) {
            auto obj = m_Objects[i];
            if (!obj.IsValid()) continue;
            auto hit = obj.GetCollider()->RaycastHit(*obj.GetTransform(), ray);
            if (hit && *hit < closest) {
                target = obj.GetTransform();
                closest = *hit;
            }
        }
        if (s_Input->IsKeyPressed(Key::MouseLeft)) {
            m_CurrentTarget = target;
        }

        if (m_CurrentTarget != nullptr) {
            Vec3 center = m_CurrentTarget->GetTranslation();
            Vec3 closest = ray.origin + glm::dot(center - ray.origin, ray.direction) * ray.direction;
            closest.y = center.y;
            Vec3 onCircle = glm::normalize(closest - center) * m_CueDistance;
            self.GetTransform()->SetTranslation(onCircle);

            Vec3 toCenter = center - onCircle;
            float angle = glm::acos(glm::dot(toCenter, ray.direction) / m_CueDistance);
            self.GetTransform()->SetRotation(-glm::pi<float>()/2, glm::cross(Vec3{0.f, 1.f, 0.f}, toCenter));
        }
     }

 private:
    float m_CueDistance = 1.f;
    Transform *m_CurrentTarget = nullptr;
    std::vector<Object> m_Objects;
    Camera *m_Camera;
};

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

    // init a model
    Model *cubeModel = new Model(cubeVertices, 8);
    cubeModel->shader = shaderProgram;
    Model *sphereModel = Model::GetSphere();
    sphereModel->shader = shaderProgram;
    Model *model = Model::loadFromFile(catSource);
    model->shader = shaderProgram;

    Material material = {
        4.f,
        Texture("/wall.png", "/wallspecular.png")
    };

    {
        auto obj = engine.NewObject();
        RenderData &renderData = obj.AddRenderData();
        renderData.model = model;

        auto &transform = obj.AddTransform(Vec3(0.f, -3.f, -8.f), Vec3(.1f, .1f, .1f), Mat4(1.0));
        transform.Rotate(1.67f, Vec3(-1.f, 0.f, 0.f));
        bindRenderData(&renderData);

        renderData.material = {
            4.f,
            Texture("/Cat_diffuse.png",
                    "/Cat_specular.png")
        };
    }

    auto aabb = engine.NewObject();
    aabb.AddTransform(Vec3(-4, 0, -3), Vec3(1), 0.0f, Vec3(1));
    aabb.AddCollider(AABB { Vec3{-0.5, -0.5, -0.5}, Vec3{0.5, 0.5, 0.5} });
    {
        auto &renderData = aabb.AddRenderData();
        renderData.model = cubeModel;
        renderData.material = material;
        bindRenderData(&renderData);
    }

    auto sphereCollider = engine.NewObject();
    sphereCollider.AddTransform(Vec3(0, 0, -3), Vec3(1), 0.0f, Vec3(1));
    sphereCollider.AddCollider(Sphere { Vec3(0.0), 1.0f });
    {
        auto &renderData = sphereCollider.AddRenderData();
        renderData.model = sphereModel;
        renderData.material = material;
        bindRenderData(&renderData);
    }

        auto mesh = engine.NewObject();
        mesh.AddTransform(Vec3(4, 0, -3), Vec3(1), 0.0f, Vec3(1));
        mesh.AddCollider(sphereModel);
    {
        auto &renderData = mesh.AddRenderData();
        renderData.model = sphereModel;
        renderData.material = material;
        bindRenderData(&renderData);
    }

    Vec3 sphereSpeed = Vec3(0, 0, -1);
    auto sphere0 = engine.NewObject();
    {
        sphere0.AddTransform(Vec3(-4, 0, 2.0), Vec3(1.0), 0, Vec3(1));
        sphere0.AddCollider(AABB { Vec3{-1, -1, -1}, Vec3{1, 1, 1} });
        auto &renderData = sphere0.AddRenderData();
        renderData.model = sphereModel;
        renderData.material = material;
        bindRenderData(&renderData);
        sphere0.AddBehaviour<MovingSphere>(aabb, sphereSpeed);
    }
    auto sphere1 = engine.NewObject();
    {
        sphere1.AddTransform(Vec3(0, 0, 2.0), Vec3(1.0), 0, Vec3(1));
        sphere1.AddCollider(AABB { Vec3{-1, -1, -1}, Vec3{1, 1, 1} });
        auto &renderData = sphere1.AddRenderData();
        renderData.model = sphereModel;
        renderData.material = material;
        bindRenderData(&renderData);
        sphere1.AddBehaviour<MovingSphere>(sphereCollider, sphereSpeed);
    }
    auto sphere2 = engine.NewObject();
    {
        sphere2.AddTransform(Vec3(4, 0, 2.0), Vec3(1.0), 0, Vec3(1));
        sphere2.AddCollider(AABB { Vec3{-1, -1, -1}, Vec3{1, 1, 1} });
        auto &renderData = sphere2.AddRenderData();
        renderData.model = sphereModel;
        renderData.material = material;
        bindRenderData(&renderData);
        sphere2.AddBehaviour<MovingSphere>(mesh, sphereSpeed);
    }

    auto pointer = engine.NewObject();
    pointer.AddTransform(Vec3(0), Vec3(1), 0, Vec3(1));
    auto &renderData = pointer.AddRenderData();
    renderData.model = Model::loadFromFile("/kiy.obj");
    renderData.model->shader = shaderProgram;
    bindRenderData(&renderData);
    renderData.material = { 4.f, Texture("/kiy.png") };
    pointer.AddBehaviour<Pointer>(std::vector<Object>{sphere0, sphere1, sphere2}, engine.camera);

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

    {
        auto pointLight = engine.NewObject();
        pointLight.AddPointLight(
            Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
            Vec3(1.0f, 1.0f, 1.0f), Vec3(-0.2, -0.5, -1.2),
            1.0f, 0.09f, 0.032f);
    }

    {
        auto pointLight = engine.NewObject();
        pointLight.AddPointLight(
            Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
            Vec3(1.0f, 1.0f, 1.0f), Vec3(2.3f, -3.3f, -4.0f),
            1, 0.09f, 0.032f);
    }

    {
        auto pointLight = engine.NewObject();
        pointLight.AddPointLight(
            Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
            Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f,  0.0f, -3.0f),
            1, 0.09f, 0.032f);
    }

    {
        auto dirLight = engine.NewObject();
        dirLight.AddDirLight(
            Vec3(0.05f, 0.05f, 0.05f), Vec3(0.4f, 0.4f, 0.4f),
            Vec3(0.5f, 0.5f, 0.5f),  Vec3(-0.2f, -1.0f, -0.3f));
    }

    {
        auto spotLight = engine.NewObject();
        spotLight.AddSpotLight(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
            Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f),
            1.0f, 0.09f, 0.032f, Vec3(0),
            glm::cos(glm::radians(12.5f)),
            glm::cos(glm::radians(15.0f)));
    }

    engine.Run();
}
