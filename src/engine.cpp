#include "engine.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#include "camera.hpp"
#include "shaders.hpp"
#include "light.hpp"
#include "material.hpp"
#include "input.hpp"
#include "texture.hpp"
#include "stb_image.h"
#include "logger.hpp"


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
std::vector<PointLight> pointLights = std::vector<PointLight>(3);
DirLight dirLight;
std::vector<SpotLight> spotLight = std::vector<SpotLight>(1);

static Input *s_Input = nullptr;

Camera* Engine::SwitchCamera(Camera* newCamera) {
    if (!newCamera) {
        Logger::Error("ENGINE::ARGUMENT_IN_SWITCHCAMERA_NULL!\n");
    }
    Camera* toReturn = camera;
    camera = newCamera;
    return toReturn;
}


Engine::Engine() {
    m_Objects = std::vector<Object *>();
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
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

    pointLights[0].ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLights[0].diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    pointLights[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLights[0].position = glm::vec3(-0.2, -0.5, -1.2);
    pointLights[0].constDistCoeff = 1;
    pointLights[0].linearDistCoeff = 0.09f;
    pointLights[0].quadraticDistCoeff = 0.032f;

    pointLights[1] = pointLights[0];
    pointLights[1].position = glm::vec3(2.3f, -3.3f, -4.0f);
    pointLights[2] = pointLights[0];
    pointLights[2].position = glm::vec3(0.0f,  0.0f, -3.0f);

    dirLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    dirLight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    dirLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);

    spotLight[0].ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    spotLight[0].diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight[0].constDistCoeff = 1.0f;
    spotLight[0].linearDistCoeff = 0.09f;
    spotLight[0].quadraticDistCoeff = 0.032f;
    spotLight[0].cutOff = glm::cos(glm::radians(12.5f));
    spotLight[0].outerCutOff = glm::cos(glm::radians(15.0f));



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
    Model * testModel = new Model(cubeVertices, 8);
    testModel->shader = shaderProgram;

    auto testObj = new Object();
    testObj->renderData = new RenderData();
    testObj->renderData->model = testModel;

    // Maybe this can be less clunky?
    // Perhaps variadic functions?
    auto images = std::vector<std::string>();
    images.push_back("/wall.png");
    images.push_back("/wallspecular.png");
    testObj->renderData->material = {
        4.f,
        Texture(images),
    };

    testObj->transform = new Transform(glm::vec3(0.f, 0.f, -3.f), glm::vec3(1.f, 1.f, 1.f), glm::mat4(1.0));

    auto render_data = testObj->renderData;

    glGenVertexArrays(1, &render_data->VAO);
    glGenBuffers(1, &render_data->VBO);
    glGenBuffers(1, &render_data->EBO);
    // bind the Vertex Array Object first,
    // then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).

    glBindVertexArray(render_data->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, render_data->VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        render_data->model->getLenArrPoints() * sizeof(float),
        render_data->model->getPoints(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        render_data->model->getLenIndices() * sizeof(unsigned int),
        render_data->model->getIndices(),
        GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(6    * sizeof(float)));
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as
    // the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as
    // the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO,
    // but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray anyways
    // so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    AddObject(testObj);
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
        camera->Update(&m_Input, deltaTime);

        while (static_cast<int>(floor(static_cast<float>(glfwGetTime()) / frameTime)) == lastRenderedFrame) {
        }

        fpsFrames++;
        lastRenderedFrame = static_cast<int>(floor(static_cast<float>(glfwGetTime()) / frameTime));
        Render(SCR_WIDTH, SCR_HEIGHT);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &render_data->VAO);
    glDeleteBuffers(1, &render_data->VBO);
    glDeleteBuffers(1, &render_data->EBO);

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

        transform->Rotate(glm::radians(0.1f), glm::radians(0.1f), glm::radians(0.1f));

        ShaderProgram shader = model->shader;
        // draw our first triangle
        shader.Use();

        glm::mat4 view = camera->GetViewMatrix();
        glm::vec3 viewPos = camera->GetPosition();

        glm::mat4 projection = glm::perspective(
                                        glm::radians(camera->GetZoom()),
                                        static_cast<float>(scr_width) / static_cast<float>(scr_height),
                                        0.1f, 100.0f);


        transform->Translate(glm::vec3(0.f, 0.f, -0.001f));

      // send matrix transform to shader
        shader.SetMat4("model", transform->GetTransformMatrix());
        shader.SetMat4("view", view);
        shader.SetVec3("viewPos", viewPos);

        // send material to shaders
        shader.SetVar("material.shininess", data->material.shininess);
        // send light to shaders
        // pointLight
        char str[100];
        for (int i = 0; i < pointLights.size(); i++) {
            snprintf(str, sizeof(str), "pointLights[%d].position", i);
            shader.SetVec3(str, pointLights[i].position);
            snprintf(str, sizeof(str), "pointLights[%d].ambient", i);
            shader.SetVec3(str, pointLights[i].ambient);
            snprintf(str, sizeof(str), "pointLights[%d].diffuse", i);
            shader.SetVec3(str, pointLights[i].diffuse);
            snprintf(str, sizeof(str), "pointLights[%d].specular", i);
            shader.SetVec3(str, pointLights[i].specular);
            snprintf(str, sizeof(str), "pointLights[%d].linearDistCoeff", i);
            shader.SetVar(str, pointLights[i].linearDistCoeff);
            snprintf(str, sizeof(str), "pointLights[%d].quadraticDistCoeff", i);
            shader.SetVar(str, pointLights[i].quadraticDistCoeff);
            snprintf(str, sizeof(str), "pointLights[%d].constDistCoeff", i);
            shader.SetVar(str, pointLights[i].constDistCoeff);
        }
        glUniform1i(shader.UniformLocation("lenArrPointL"), pointLights.size());
        // directionLight
        shader.SetVec3("dirLight.ambient", dirLight.ambient);
        shader.SetVec3("dirLight.specular", dirLight.specular);
        shader.SetVec3("dirLight.direction", dirLight.direction);
        shader.SetVec3("dirLight.diffuse", dirLight.diffuse);
        // spotLight
        for (int i = 0; i < spotLight.size(); i++) {
            snprintf(str, sizeof(str), "spotLight[%d].diffuse", i);
            shader.SetVec3(str, spotLight[i].diffuse);
            snprintf(str, sizeof(str), "spotLight[%d].direction", i);
            shader.SetVec3(str, camera->GetFront());
            snprintf(str, sizeof(str), "spotLight[%d].ambient", i);
            shader.SetVec3(str, spotLight[i].ambient);
            snprintf(str, sizeof(str), "spotLight[%d].position", i);
            shader.SetVec3(str, camera->GetPosition());
            snprintf(str, sizeof(str), "spotLight[%d].specular", i);
            shader.SetVec3(str, spotLight[i].specular);
            snprintf(str, sizeof(str), "spotLight[%d].cutOff", i);
            shader.SetVar(str, spotLight[i].cutOff);
            snprintf(str, sizeof(str), "spotLight[%d].linearDistCoeff", i);
            shader.SetVar(str, spotLight[i].linearDistCoeff);
            snprintf(str, sizeof(str), "spotLight[%d].outerCutOff", i);
            shader.SetVar(str, spotLight[i].outerCutOff);
            snprintf(str, sizeof(str), "spotLight[%d].constDistCoeff", i);
            shader.SetVar(str, spotLight[i].constDistCoeff);
            snprintf(str, sizeof(str), "spotLight[%d].quadraticDistCoeff", i);
            shader.SetVar(str, spotLight[i].quadraticDistCoeff);
        }
        glUniform1i(shader.UniformLocation("lenArrSpotL"), spotLight.size());
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
