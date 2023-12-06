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
    Object* testObj = initModel();
    engine.AddObject<>(testObj);
    engine.Run(SCR_WIDTH, SCR_HEIGHT);
}

Object* initModel() {
    // build and compile our shader program ------------------------------------
    Shader vShader = Shader(VertexShader, vertexShaderSource);
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);

    // init a model
    Model * model = Model::loadFromFile(catSource);

    auto obj = new Object();
    obj->renderData = new RenderData();
    obj->renderData->model = model;
    ShaderProgram shaderProgram = ShaderProgram(vShader, fShader);
    obj->renderData->model->setShader(shaderProgram);

    obj->transform = new Transform(glm::vec3(0.f, -3.f, -8.f), glm::vec3(.1f, .1f, .1f), glm::mat4(1.0));
    obj->transform->Rotate(1.67f, glm::vec3(-1.f, 0.f, 0.f));
    auto render_data = obj->renderData;

    bindRenderData(render_data);

    // Maybe this can be less clunky?
    // Perhaps variadic functions?
    auto images = std::vector<std::string>();
    images.push_back("/cat_diffuse.png");
    images.push_back("/cat_specular.png");
    obj->renderData->material = {
        4.f,
        Texture(images),
    };

    return obj;
}
