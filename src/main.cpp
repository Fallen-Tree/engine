#include "engine.hpp"
#include "collisions.hpp"
#include "logger.hpp"

extern Input *s_Input;

static int SCR_HEIGHT = 600;
static int SCR_WIDTH = 800;

const char *cubeSource = "/cube2.obj";
const char *catSource = "/cat.obj";
const char *benchSource = "/bench.obj";

const char *vertexShaderSource = "/vertex/standart.vshader";
const char *fragmentShaderSource = "/fragment/standart.fshader";

class MovingSphere : public Object {
 public:
    void Update(float dt) override {
        if (!collider->Collide(*transform, m_Target->collider, *m_Target->transform)) {
            transform->Translate(m_Speed * dt);
        }
    }

    MovingSphere(Object *target, Vec3 speed) {
        m_Target = target;
        m_Speed = speed;
    }
 private:
    Object *m_Target;
    Vec3 m_Speed;
};

class Pointer : public Object {
 public:
     Pointer(std::vector<Object *> objects, Camera *camera) {
         m_Objects = objects;
         m_Camera = camera;
     }

    void Update(float dt) override {
        Ray ray = m_Camera->GetRayThroughScreenPoint({s_Input->MouseX(), s_Input->MouseY()});
        ray.direction = glm::normalize(ray.direction);
        /* transform->SetTranslation(ray.origin + 2.f * ray.direction); */
        /* Logger::Info("(%f %f %f)", ray.origin.x, ray.origin.y, ray.origin.z); */
        /* Logger::Info("(%f %f %f)", ray.direction.x, ray.direction.y, ray.direction.z); */
        /* Ray ray = Ray { m_Camera->GetFront(), m_Camera->GetPosition() }; */
        for (int i = 0; i < m_Objects.size(); i++) {
            auto obj = m_Objects[i];
            if (obj->collider->Raycast(*obj->transform, ray)) {
                Logger::Info("Hit at %d!", i);
            }
        }
     }

 private:
     std::vector<Object *> m_Objects;
     Camera *m_Camera;
};

Object* initModel();

int main() {
    auto engine = Engine(SCR_WIDTH, SCR_HEIGHT);
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
    auto render_data = obj->renderData;

    bindRenderData(render_data);

    auto imagesCat = std::vector<std::string>();
    imagesCat.push_back("/Cat_diffuse.png");
    imagesCat.push_back("/Cat_specular.png");
    obj->renderData->material = {
        4.f,
        Texture(imagesCat),
    };
    engine.AddObject<>(obj);

    // Maybe this can be less clunky?
    // Perhaps variadic functions?
    auto imagesCube = std::vector<std::string>();
    imagesCube.push_back("/wall.png");
    imagesCube.push_back("/wallspecular.png");
    Material material = {
        4.f,
        Texture(imagesCube),
    };

    auto setUpObj = [=, &engine](Transform transform, auto primitive, Model *model) {
        auto obj = new Object();
        obj->renderData = new RenderData();
        auto renderData = obj->renderData;
        renderData->model = model;
        renderData->material = material;
        bindRenderData(renderData);

        obj->transform = new Transform(transform);
        obj->collider = new Collider { primitive };
        engine.AddObject<>(obj);
        return obj;
    };

    auto aabb = setUpObj(
        Transform(Vec3(-4, 0, -3), Vec3(1.0), 0, Vec3(1)),
        AABB {
            Vec3{-0.5, -0.5, -0.5},
            Vec3{0.5, 0.5, 0.5},
        },
        cubeModel);

    auto sphere = setUpObj(
        Transform(Vec3(0, 0, -3), Vec3(1.0), 0, Vec3(1)),
        Sphere { Vec3(0.0), 1.0f },
        sphereModel);

    auto mesh = setUpObj(
        Transform(Vec3(4.0, 0, -3), Vec3(1.0), 0, Vec3(1)),
        sphereModel,
        sphereModel);

    auto getSphereObj = [=](Transform transform, Object *target, Vec3 speed) {
        auto obj = new MovingSphere(target, speed);
        obj->renderData = new RenderData();
        auto renderData = obj->renderData;
        renderData->model = sphereModel;
        renderData->material = material;
        bindRenderData(renderData);

        obj->transform = new Transform(transform);

        obj->collider = new Collider{Sphere{
            Vec3(0.0),
            1.0f,
        }};
        return obj;
    };

    Object *spheres[3] = {
        getSphereObj(
            Transform(Vec3(-4, 0, 2.0), Vec3(1.0), 0, Vec3(1)),
            aabb,
            Vec3(0, 0, -1)),
        getSphereObj(
            Transform(Vec3(0, 0, 2.0), Vec3(1.0), 0, Vec3(1)),
            sphere,
            Vec3(0, 0, -1)),
        getSphereObj(
            Transform(Vec3(4, 0, 2.0), Vec3(1.0), 0, Vec3(1)),
            mesh,
            Vec3(0, 0, -1)),
    };
    engine.AddObject<>(spheres[0]);
    engine.AddObject<>(spheres[1]);
    engine.AddObject<>(spheres[2]);

    auto observer = new Pointer(
        {spheres[0], spheres[1], spheres[2], aabb, sphere},
        engine.camera);
    /* observer->transform = new Transform(Vec3(0), Vec3(0.5f), Mat4(1)); */
    /* observer->renderData = new RenderData(); */
    /* auto renderData = observer->renderData; */
    /* renderData->model = sphereModel; */
    /* renderData->material = material; */
    /* bindRenderData(renderData); */
    engine.AddObject<>(observer);

    engine.Run(SCR_WIDTH, SCR_HEIGHT);
}
