#include <limits>
#include "engine.hpp"
#include "collisions.hpp"
#include "logger.hpp"
#include <glm/gtx/string_cast.hpp>

const char *cubeSource = "/cube2.obj";
const char *catSource = "/cat.obj";
const char *benchSource = "/bench.obj";

const char *vertexShaderSource = "/standart.vshader";
const char *fragmentShaderSource = "/standart.fshader";

class MovingSphere : public Object {
 public:

    MovingSphere() {
    }
};

class Pointer : public Object {
 public:
     Pointer(std::vector<Object *> objects, Camera *camera) {
         m_Objects = objects;
         m_Camera = camera;
     }

    void Update(float dt) override {
        Ray ray = m_Camera->GetRayThroughScreenPoint({s_Input->MouseX(), s_Input->MouseY()});
        Transform *target = nullptr;
        float closest = std::numeric_limits<float>::max();
        for (int i = 0; i < m_Objects.size(); i++) {
            auto obj = m_Objects[i];
            auto hit = obj->collider->RaycastHit(*obj->transform, ray);
            if (hit && *hit < closest) {
                target = obj->transform;
                closest = *hit;
            }
        }
        if (s_Input->IsKeyPressed(Key::MouseLeft))
            m_CurrentTarget = target;

        if (m_CurrentTarget != nullptr) {
            Vec3 center = m_CurrentTarget->GetTranslation();
            Vec3 closest = ray.origin + glm::dot(center - ray.origin, ray.direction) * ray.direction;
            closest.y = center.y;
            Vec3 onCircle = glm::normalize(closest - center) * m_CueDistance;
            transform->SetTranslation(onCircle);

            Vec3 toCenter = center - onCircle;
            float angle = glm::acos(glm::dot(toCenter, ray.direction) / m_CueDistance);
            transform->SetRotation(-glm::pi<float>()/2, glm::cross(Vec3{0.f, 1.f, 0.f}, toCenter));
        
        }
        // Logger::Info("coord: %s", glm::to_string(transform->GetTranslation()));
     }

 private:
    float m_CueDistance = 1.f;
    Transform *m_CurrentTarget = nullptr;
    std::vector<Object *> m_Objects;
    Camera *m_Camera;
};

// Object* initModel();

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
    obj->renderData = new RenderData();
    obj->renderData->model = model;
    obj->renderData->model->shader = shaderProgram;

    obj->transform = new Transform(Vec3(0.f, -3.f, -8.f), Vec3(.1f, .1f, .1f), Mat4(1.0));
    obj->transform->Rotate(1.67f, Vec3(-1.f, 0.f, 0.f));
    obj->collider = new Collider{AABB {
            Vec3{-1, -1, -1},
            Vec3{1, 1, 1},
        }};
    auto render_data = obj->renderData;

    bindRenderData(render_data);

    obj->renderData->material = {
        4.f,
        Texture("/Cat_diffuse.png",
                "/Cat_specular.png")
    };
    engine.AddObject<>(obj);

    Material material = {
        4.f,
        Texture("/wall.png", "/wallspecular.png")
    };

    auto setUpObj = [=, &engine](Transform transform, auto primitive, Model *model) {
        auto obj = new Object();
        obj->renderData = new RenderData();
        auto renderData = obj->renderData;
        renderData->model = model;
        renderData->material = material;
        bindRenderData(renderData);

        obj->transform = new Transform(transform);
        //obj->rigidbody = new RigidBody(1, Mat3(1), Vec3(0), NONE, 
        //    1, Vec3(0), Vec3(0));
        obj->collider = new Collider{AABB {
            Vec3{-1, -1, -1},
            Vec3{1, 1, 1},
        }};
        engine.AddObject<>(obj);
        return obj;
    };

    auto aabb = setUpObj(
        Transform(Vec3(0, -65, 0), Vec3(100.0), 0, Vec3(1)),
        AABB {
            Vec3{-0.5, -0.5, -0.5},
            Vec3{0.5, 0.5, 0.5},
        },
        cubeModel);

    auto getSphereObj = [=](Transform transform, Vec3 speed) {
        auto obj = new MovingSphere();
        obj->renderData = new RenderData();
        auto renderData = obj->renderData;
        renderData->model = sphereModel;
        renderData->material = material;
        bindRenderData(renderData);

        obj->transform = new Transform(transform);

        obj->collider = new Collider{AABB {
            Vec3{-1, -1, -1},
            Vec3{1, 1, 1},
        }};
        //obj->rigidbody = new RigidBody(1, Mat3(1), speed, LINEAR, 
        //    1, Vec3(0), Vec3(0));
        return obj;
    };

    Object *spheres[2] = {
        getSphereObj(
            Transform(Vec3(0, 0, 2.0), Vec3(1.0), 0, Vec3(0, 0, 1)),
            Vec3(0, 0, -1)),
        getSphereObj(
            Transform(Vec3(4, 0, 2.0), Vec3(1.0), 0, Vec3(0, 1, 1)),
            Vec3(0, 0, -1)),
    };
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
    fpsObj->text = new Text(textOcra, "", 685.0f, 575.0f, 1.f, Vec3(0, 0, 0));
    engine.AddObject<>(fpsObj);

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
