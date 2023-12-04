#include "engine.hpp"

static int SCR_HEIGHT = 600;
static int SCR_WIDTH = 800;

const char *vertexShaderSource = "/vertex/standart.vshader";
const char *fragmentShaderSource = "/fragment/standart.fshader";


Object* initModel();

int main() {
    auto engine = Engine(SCR_WIDTH, SCR_HEIGHT);
    Object* testObj = initModel();
    engine.AddObject<>(testObj);

    // init light objects
    Object* pointLight1 = new Object();
    pointLight1->light = new PointLight;
    auto pointLight = std::get<PointLight*>(pointLight1->light);
    pointLight->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight->diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    pointLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight->position = glm::vec3(-0.2, -0.5, -1.2);
    pointLight->constDistCoeff = 1;
    pointLight->linearDistCoeff = 0.09f;
    pointLight->quadraticDistCoeff = 0.032f;
    engine.AddObject<>(pointLight1);

    Object* pointLight2 = new Object();
    pointLight2->light = new PointLight;
    pointLight = std::get<PointLight*>(pointLight2->light);
    pointLight->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight->diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    pointLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight->position = glm::vec3(2.3f, -3.3f, -4.0f);
    pointLight->constDistCoeff = 1;
    pointLight->linearDistCoeff = 0.09f;
    engine.AddObject<>(pointLight2);

    Object* pointLight3 = new Object();
    pointLight3->light = new PointLight;
    pointLight = std::get<PointLight*>(pointLight3->light);
    pointLight->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight->diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    pointLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight->position = glm::vec3(0.0f,  0.0f, -3.0f);
    pointLight->constDistCoeff = 1;
    pointLight->linearDistCoeff = 0.09f;
    engine.AddObject<>(pointLight3);

    Object* dirLight = new Object();
    dirLight->light = new DirLight;
    auto directionLight = std::get<DirLight*>(dirLight->light);
    directionLight->ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    directionLight->diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    directionLight->specular = glm::vec3(0.5f, 0.5f, 0.5f);
    directionLight->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    engine.AddObject<>(dirLight);

    Object* spotLight = new Object();
    spotLight->light = new SpotLight;
    auto sptLight = std::get<SpotLight*>(spotLight->light);
    sptLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    sptLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    sptLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    sptLight->constDistCoeff = 1.0f;
    sptLight->linearDistCoeff = 0.09f;
    sptLight->quadraticDistCoeff = 0.032f;
    sptLight->cutOff = glm::cos(glm::radians(12.5f));
    sptLight->outerCutOff = glm::cos(glm::radians(15.0f));
    engine.AddObject<>(spotLight);

    engine.Run(SCR_WIDTH, SCR_HEIGHT);
}

Object* initModel() {
    // build and compile our shader program ------------------------------------
    Shader vShader = Shader(VertexShader, vertexShaderSource);
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);

    // ShaderProgram shaderProgram = ShaderProgram(vShader, fShader2);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
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

    // init a model
    Model * model = new Model(cubeVertices, 8);

    auto obj = new Object();
    obj->renderData = new RenderData();
    obj->renderData->model = model;
    obj->renderData->model->shader = new ShaderProgram(vShader, fShader);

    obj->transform = new Transform(glm::vec3(0.f, 0.f, -3.f), glm::vec3(1.f, 1.f, 1.f), glm::mat4(1.0));

    auto render_data = obj->renderData;

    bindRenderData(render_data);

    // Maybe this can be less clunky?
    // Perhaps variadic functions?
    auto images = std::vector<std::string>();
    images.push_back("/wall.png");
    images.push_back("/wallspecular.png");
    obj->renderData->material = {
        4.f,
        Texture(images),
    };

    return obj;
}
