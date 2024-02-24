#include <limits>
#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "collisions.hpp"
#include "logger.hpp"

const char *cubeSource = "/cube2.obj";
const char *catSource = "/cat.obj";
const char *benchSource = "/bench.obj";

const char *vertexShaderSource = "/standart.vshader";
const char *fragmentShaderSource = "/standart.fshader";

class MovingSphere : public Behaviour {
 public:
    void Update(float dt) override {
        if (!self.GetCollider()->Collide(*self.GetTransform(), m_Target.GetCollider(), *m_Target.GetTransform())) {
            self.GetTransform()->Translate(m_Speed * dt);
        }
    }

    MovingSphere(Object self, Object target, Vec3 speed) : Behaviour(self), m_Target(target) {
        m_Speed = speed;
    }
 private:
    Object m_Target;
    Vec3 m_Speed;
};

/* class Pointer : public Object { */
/*  public: */
/*      Pointer(std::vector<Object *> objects, Camera *camera) { */
/*          m_Objects = objects; */
/*          m_Camera = camera; */
/*      } */

/*     void Update(float dt) override { */
/*         Ray ray = m_Camera->GetRayThroughScreenPoint({s_Input->MouseX(), s_Input->MouseY()}); */
/*         Transform *target = nullptr; */
/*         float closest = std::numeric_limits<float>::max(); */
/*         for (int i = 0; i < m_Objects.size(); i++) { */
/*             auto obj = m_Objects[i]; */
/*             auto hit = obj->collider->RaycastHit(*obj->transform, ray); */
/*             if (hit && *hit < closest) { */
/*                 target = obj->transform; */
/*                 closest = *hit; */
/*             } */
/*         } */
/*         if (s_Input->IsKeyPressed(Key::MouseLeft)) */
/*             m_CurrentTarget = target; */

/*         if (m_CurrentTarget != nullptr) { */
/*             Vec3 center = m_CurrentTarget->GetTranslation(); */
/*             Vec3 closest = ray.origin + glm::dot(center - ray.origin, ray.direction) * ray.direction; */
/*             closest.y = center.y; */
/*             Vec3 onCircle = glm::normalize(closest - center) * m_CueDistance; */
/*             transform->SetTranslation(onCircle); */

/*             Vec3 toCenter = center - onCircle; */
/*             float angle = glm::acos(glm::dot(toCenter, ray.direction) / m_CueDistance); */
/*             transform->SetRotation(-glm::pi<float>()/2, glm::cross(Vec3{0.f, 1.f, 0.f}, toCenter)); */
/*         } */
/*      } */

/*  private: */
/*     float m_CueDistance = 1.f; */
/*     Transform *m_CurrentTarget = nullptr; */
/*     std::vector<Object *> m_Objects; */
/*     Camera *m_Camera; */
/* }; */

/* Object* initModel(); */

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

    {
        auto aabb = engine.NewObject();
        aabb.AddTransform(Vec3(-4, 0, -3), Vec3(1), 0.0f, Vec3(1));
        aabb.AddCollider(AABB { Vec3{-0.5, -0.5, -0.5}, Vec3{0.5, 0.5, 0.5} });
        auto &renderData = aabb.AddRenderData();
        renderData.model = cubeModel;
        renderData.material = material;
        bindRenderData(&renderData);
    }

    {
        auto sphere = engine.NewObject();
        sphere.AddTransform(Vec3(0, 0, -3), Vec3(1), 0.0f, Vec3(1));
        sphere.AddCollider(Sphere { Vec3(0.0), 1.0f });
        auto &renderData = sphere.AddRenderData();
        renderData.model = sphereModel;
        renderData.material = material;
        bindRenderData(&renderData);
    }

/*     auto mesh = setUpObj( */
/*         Transform(Vec3(4.0, 0, -3), Vec3(1.0), 0, Vec3(1)), */
/*         sphereModel, */
/*         sphereModel); */

/*     auto getSphereObj = [=](Transform transform, Object *target, Vec3 speed) { */
/*         auto obj = new MovingSphere(target, speed); */
/*         obj->renderData = new RenderData(); */
/*         auto renderData = obj->renderData; */
/*         renderData->model = sphereModel; */
/*         renderData->material = material; */
/*         bindRenderData(renderData); */

/*         obj->transform = new Transform(transform); */

/*         obj->collider = new Collider{AABB { */
/*             Vec3{-1, -1, -1}, */
/*             Vec3{1, 1, 1}, */
/*         }}; */
/*         return obj; */
/*     }; */

/*     Object *spheres[3] = { */
/*         getSphereObj( */
/*             Transform(Vec3(-4, 0, 2.0), Vec3(1.0), 0, Vec3(1)), */
/*             aabb, */
/*             Vec3(0, 0, -1)), */
/*         getSphereObj( */
/*             Transform(Vec3(0, 0, 2.0), Vec3(1.0), 0, Vec3(1)), */
/*             sphere, */
/*             Vec3(0, 0, -1)), */
/*         getSphereObj( */
/*             Transform(Vec3(4, 0, 2.0), Vec3(1.0), 0, Vec3(1)), */
/*             mesh, */
/*             Vec3(0, 0, -1)), */
/*     }; */
/*     engine.AddObject<>(spheres[0]); */
/*     engine.AddObject<>(spheres[1]); */
/*     engine.AddObject<>(spheres[2]); */

/*     auto observer = new Pointer( */
/*         {spheres[0], spheres[1], spheres[2], aabb, sphere}, */
/*         engine.camera); */

/*     observer->renderData = new RenderData(); */
/*     auto renderData = observer->renderData; */
/*     renderData->model = Model::loadFromFile("/kiy.obj"); */
/*     renderData->model->shader = shaderProgram; */
/*     bindRenderData(renderData); */

/*     observer->transform = new Transform(Vec3(0), Vec3(1), 0, Vec3(1)); */
/*     observer->renderData->material = { */
/*         4.f, */
/*         Texture("/kiy.png") */
/*     }; */
/*     engine.AddObject<>(observer); */

    class FpsText : public Behaviour {
     public:
        FpsText(Object self) : Behaviour(self) {}
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
