#include "engine.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#include "math_types.hpp"
#include "camera.hpp"
#include "shaders.hpp"
#include "light.hpp"
#include "material.hpp"
#include "input.hpp"
#include "texture.hpp"
#include "stb_image.h"
#include "logger.hpp"
#include "collisions.hpp"
#include "glm/gtx/string_cast.hpp"


int viewportWidth, viewportHeight;
// Left bottom corner coordinates of viewport
int viewportStartX, viewportStartY;
// For resoliton and initial window size. 1600x900 for example.
int scrWidth, scrHeight;

// should send to all constants
const int maxValidKey = 350;
const float fpsLimit = 500;
const float fpsShowingInterval = 1.f;

static Engine *s_Engine = nullptr;
EnvLight envL;

static Input *s_Input = nullptr;

Engine::Engine() {
    m_Objects = std::vector<Object *>();
    m_Camera = Camera(Vec3(0.0f, 0.0f, 3.0f));
    s_Engine = this;
}

Engine::~Engine() {
    std::cout << "Goodbye";
}

void Engine::AddObject(Object *a) {
    m_Objects.push_back(a);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


const char *vertexShaderSource = "/vertex/standart.vshader";
const char *fragmentShaderSource1 = "/fragment/green.fshader";
const char *fragmentShaderSource2 = "/fragment/red.fshader";
const char *fragmentShaderSource3 = "/fragment/blue.fshader";

void Engine::Run(int SCR_WIDTH, int SCR_HEIGHT) {
    scrWidth = SCR_WIDTH;
    scrHeight = SCR_HEIGHT;
    viewportWidth = SCR_WIDTH;
    viewportHeight = SCR_HEIGHT;
    viewportStartX = 0;
    viewportStartY = 0;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    m_Window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (m_Window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
    glfwSetScrollCallback(m_Window, scroll_callback);
    glfwSetKeyCallback(m_Window, key_callback);

    m_Input.SetWindow(m_Window);
    m_Input.SetMode(MODE, VALUE);
    m_Input.InitMouse();

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }


    // build and compile our shader program ------------------------------------
    Shader vShader = Shader(VertexShader, vertexShaderSource);
    Shader fShader1 = Shader(FragmentShader, fragmentShaderSource1);
    Shader fShader2 = Shader(FragmentShader, fragmentShaderSource2);
    Shader fShader3 = Shader(FragmentShader, fragmentShaderSource3);

    ShaderProgram shaderProgram = ShaderProgram(vShader, fShader2);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    envL.m_Ambient = Vec3(0.2f, 0.2f, 0.2f);
    envL.m_Diffuse = Vec3(0.5f, 0.5f, 0.5f);
    envL.m_Specular = Vec3(1.0f, 1.0f, 1.0f);
    envL.m_Position = Vec3(-0.2, -0.5, -1.2);

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
    Model *cubeModel = new Model(cubeVertices, 8);
    cubeModel->shader = shaderProgram;
    Model *sphereModel = Model::GetSphere();
    sphereModel->shader = shaderProgram;

    // Maybe this can be less clunky?
    // Perhaps variadic functions?
    auto images = std::vector<std::string>();
    images.push_back("/wall.png");
    images.push_back("/wallspecular.png");
    Material material = {
        4.f,
        Texture(images),
    };

    auto getSphereObj = [=](Transform transform) {
        auto obj = new Object();
        obj->renderData = new RenderData();
        auto renderData = obj->renderData;
        renderData->model = sphereModel;
        renderData->material = material;
        glGenVertexArrays(1, &renderData->VAO);
        glGenBuffers(1, &renderData->VBO);
        glGenBuffers(1, &renderData->EBO);

        glBindVertexArray(renderData->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, renderData->VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            renderData->model->getLenArrPoints() * sizeof(float),
            renderData->model->getPoints(),
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData->EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            renderData->model->getLenIndices() * sizeof(unsigned int),
            renderData->model->getIndices(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            reinterpret_cast<void*>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

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
    AddObject(spheres[0]);
    AddObject(spheres[1]);
    AddObject(spheres[2]);

    auto aabb = new Object();
    aabb->renderData = new RenderData();
    auto renderData = aabb->renderData;
    renderData->model = cubeModel;
    renderData->material = material;
    glGenVertexArrays(1, &renderData->VAO);
    glGenBuffers(1, &renderData->VBO);
    glGenBuffers(1, &renderData->EBO);

    glBindVertexArray(renderData->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderData->VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        renderData->model->getLenArrPoints() * sizeof(float),
        renderData->model->getPoints(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData->EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        renderData->model->getLenIndices() * sizeof(unsigned int),
        renderData->model->getIndices(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    aabb->transform = new Transform(Vec3(-4, 0, -3), Vec3(1.0), 0, Vec3(1));
    aabb->collider = new Collider{AABB {
        Vec3{-0.5, -0.5, -0.5},
        Vec3{0.5, 0.5, 0.5},
    }};
    AddObject(aabb);

    auto sphere = getSphereObj(Transform(Vec3(0, 0, -3), Vec3(1.0), 0, Vec3(1)));
    AddObject(sphere);

    auto mesh = getSphereObj(Transform(Vec3(4.0, 0, -3), Vec3(1.0), 0, Vec3(1)));
    mesh->collider = new Collider{sphereModel};
    AddObject(mesh);

    glEnable(GL_DEPTH_TEST);

    float lastFpsShowedTime = 0.f;
    int lastRenderedFrame = -1;
    int fpsFrames = 0;
    const float frameTime = 1.f / fpsLimit;
    float deltaTime = 0.0f;
    float lastTime = 0.0f;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(m_Window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (currentTime - lastFpsShowedTime > fpsShowingInterval) {
            Logger::Info("FPS: %d", static_cast<int>(fpsFrames / (currentTime - lastFpsShowedTime)));
            lastFpsShowedTime = currentTime;
            fpsFrames = 0;
        }

        m_Input.Update();
        glfwPollEvents();
        processInput(m_Window);
        m_Camera.Update(&m_Input, deltaTime);

        if (!spheres[0]->collider->Collide(
                *spheres[0]->transform,
                aabb->collider, *aabb->transform)) {
            spheres[0]->transform->Translate(Vec3(0.f, 0.f, -0.001f));
        }
        if (!spheres[1]->collider->Collide(
                *spheres[1]->transform,
                sphere->collider, *sphere->transform)) {
            spheres[1]->transform->Translate(Vec3(0.f, 0.f, -0.001f));
        }

        if (!spheres[2]->collider->Collide(
                *spheres[2]->transform,
                mesh->collider, *mesh->transform)) {
            spheres[2]->transform->Translate(Vec3(0.f, 0.f, -0.001f));
        }

        while (static_cast<int>(floor(static_cast<float>(glfwGetTime()) / frameTime)) == lastRenderedFrame) {
        }

        fpsFrames++;
        lastRenderedFrame = static_cast<int>(floor(static_cast<float>(glfwGetTime()) / frameTime));
        Render(SCR_WIDTH, SCR_HEIGHT);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &renderData->VAO);
    glDeleteBuffers(1, &renderData->VBO);
    glDeleteBuffers(1, &renderData->EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return;
}

void Engine::Render(int scr_width, int scr_height) {
    // render
    // ------

    // Coloring all window (black)
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Coloring only viewport (sky)
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewportStartX, viewportStartY, viewportWidth, viewportHeight);
    glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    for (uint64_t i = 0; i < m_Objects.size(); i++) {
        auto object = m_Objects[i];
        if (!object->renderData || !object->transform)
            continue;

        auto data = object->renderData;
        auto transform = object->transform;

        auto model = data->model;

        float timeValue = glfwGetTime();

        ShaderProgram shader = model->shader;
        // draw our first triangle
        shader.Use();

        Mat4 view = m_Camera.GetViewMatrix();
        Vec3 viewPos = m_Camera.GetPosition();

        Mat4 projection = glm::perspective(
                                        glm::radians(m_Camera.GetZoom()),
                                        static_cast<float>(scr_width) / static_cast<float>(scr_height),
                                        0.1f, 100.0f);
      // send matrix transform to shader
        shader.SetMat4("model", transform->GetTransformMatrix());
        shader.SetMat4("view", view);
        shader.SetVec3("viewPos", viewPos);

        // send material to shaders
        shader.SetVar("material.shininess", data->material.shininess);
        // send light to shaders
        shader.SetVec3("light.position", envL.m_Position);
        shader.SetVec3("light.ambient", envL.m_Ambient);
        shader.SetVec3("light.diffuse", envL.m_Diffuse);
        shader.SetVec3("light.specular", envL.m_Specular);
        // send inf about texture
        data->material.texture.bind();
        glUniform1i(shader.UniformLocation("material.duffuse"), 0);
        glUniform1i(shader.UniformLocation("material.specular"), 1);

        // Note: currently we set the projection matrix each frame,
        // but since the projection matrix rarely changes it's
        // often best practice to set it outside the main loop only once.
        shader.SetMat4("projection", projection);

        glBindVertexArray(data->VAO);

        glDrawElements(GL_TRIANGLES, model->getLenIndices(), GL_UNSIGNED_INT, 0);
    }
    glfwSwapBuffers(m_Window);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow *window) {
    if (s_Engine->m_Input.IsKeyPressed(Key::Escape))
        glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key > 0 && key < maxValidKey && action == GLFW_PRESS) {
        s_Engine->m_Input.ButtonPress(key);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.


    // width and height have to be the same resolution as scr_width and scr_height
    viewportWidth = width;
    viewportHeight = height;

    float current = static_cast<float>(width) / static_cast<float>(height);
    float expected = static_cast<float>(scrWidth) / static_cast<float>(scrHeight);

    if (current > expected) {
        viewportWidth = scrWidth * height / scrHeight;
    } else {
        viewportHeight = scrHeight * width / scrWidth;
    }

    Logger::Info("Window resized, current viewport width and height: %d, %d.", viewportWidth, viewportHeight);

    viewportStartX = (width - viewportWidth) / 2;
    viewportStartY = (height - viewportHeight) / 2;

    glViewport(viewportStartX, viewportStartY, viewportWidth, viewportHeight);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    s_Engine->m_Input.SetScrollOffset(static_cast<float>(yoffset));
}
