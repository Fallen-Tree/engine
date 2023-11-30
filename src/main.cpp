#include "engine.hpp"

static int SCR_HEIGHT = 600;
static int SCR_WIDTH = 800;

const char *cubeSource = "/cube2.obj";
const char *catSource = "/cat.obj";
const char *benchSource = "/bench.obj";

const char *vertexShaderSource = "/vertex/standart.vshader";
const char *fragmentShaderSource = "/fragment/standart.fshader";


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

    obj->transform = new Transform(glm::vec3(0.f, -3.f, -8.f), glm::vec3(.1f, .1f, .1f), glm::mat4(1.0));
    obj->transform->Rotate(1.67f, glm::vec3(-1.f, 0.f, 0.f));
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

    auto getSphereObj = [=](Transform transform) {
        auto obj = new Object();
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
        getSphereObj(Transform(Vec3(-4, 0, 2.0), Vec3(1.0), 0, Vec3(1))),
        getSphereObj(Transform(Vec3(0, 0, 2.0), Vec3(1.0), 0, Vec3(1))),
        getSphereObj(Transform(Vec3(4, 0, 2.0), Vec3(1.0), 0, Vec3(1))),
    };
    engine.AddObject<>(spheres[0]);
    engine.AddObject<>(spheres[1]);
    engine.AddObject<>(spheres[2]);

    auto aabb = new Object();
    aabb->renderData = new RenderData();
    auto renderData = aabb->renderData;
    renderData->model = cubeModel;
    renderData->material = material;
    bindRenderData(renderData);

    aabb->transform = new Transform(Vec3(-4, 0, -3), Vec3(1.0), 0, Vec3(1));
    aabb->collider = new Collider{AABB {
        Vec3{-0.5, -0.5, -0.5},
        Vec3{0.5, 0.5, 0.5},
    }};
    engine.AddObject<>(aabb);

    auto sphere = getSphereObj(Transform(Vec3(0, 0, -3), Vec3(1.0), 0, Vec3(1)));
    engine.AddObject<>(sphere);

    auto mesh = getSphereObj(Transform(Vec3(4.0, 0, -3), Vec3(1.0), 0, Vec3(1)));
    mesh->collider = new Collider{sphereModel};
    engine.AddObject<>(mesh);

    engine.Run(SCR_WIDTH, SCR_HEIGHT);
}
