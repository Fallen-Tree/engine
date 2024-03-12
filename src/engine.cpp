#include "engine.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <irrKlang.h>
#include <vector>
#include <iostream>

#include "math_types.hpp"
#include "camera.hpp"
#include "shaders.hpp"
#include "light.hpp"
#include "material.hpp"
#include "input.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "stb_image.h"
#include "logger.hpp"
#include "collisions.hpp"
#include "animation.hpp"
#include <glm/gtx/string_cast.hpp>


int viewportWidth, viewportHeight;
// Left bottom corner coordinates of viewport
int viewportStartX, viewportStartY;
// For resoliton and initial window size. 1600x900 for example.
int scrWidth, scrHeight;

static Engine *s_Engine = nullptr;
Input *s_Input = nullptr;

unsigned int fps = 0;

Camera* Engine::SwitchCamera(Camera* newCamera) {
    if (!newCamera) {
        Logger::Error("ENGINE::ARGUMENT_IN_SWITCHCAMERA_NULL!\n");
    }
    Camera* toReturn = camera;
    camera = newCamera;
    return toReturn;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void processInput(GLFWwindow *window);

Engine::Engine() {
    m_Objects = std::vector<Object *>();
    camera = new Camera(Vec3(0.0f, 0.0f, 3.0f));
    s_Engine = this;

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
    glfwSetMouseButtonCallback(m_Window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}

Engine::~Engine() {
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    for (int i = 0; i < m_Objects.size(); i++) {
        if (m_Objects[i] == nullptr || m_Objects[i]->renderData == nullptr)
            continue;
        glDeleteVertexArrays(1, &m_Objects[i]->renderData->VAO);
        glDeleteBuffers(1, &m_Objects[i]->renderData->VBO);
        glDeleteBuffers(1, &m_Objects[i]->renderData->EBO);
    }
    std::cout << "Goodbye";
}

void Engine::AddObject(Object *a) {
    if (a != nullptr) {
        switch (a->light.index()) {
            case 0:
                m_DirLights.push_back(std::get<DirLight*>(a->light));
                break;
            case 1:
                m_PointLights.push_back(std::get<PointLight*>(a->light));
                break;
            case 2:
                m_SpotLights.push_back(std::get<SpotLight*>(a->light));
                break;
            case 3:
                break;
            default:
                Logger::Error("ENGINE::ADD_OBJECT::INVALID_TYPE_OF_OBJEC");
        }
    }
    m_Objects.push_back(a);
}

void Engine::Run() {
    scrWidth = SCR_WIDTH;
    scrHeight = SCR_HEIGHT;
    viewportWidth = SCR_WIDTH;
    viewportHeight = SCR_HEIGHT;
    viewportStartX = 0;
    viewportStartY = 0;
    s_Input = &m_Input;
    m_Input.SetWindow(m_Window);
    m_Input.SetMode(MODE, VALUE);
    m_Input.InitMouse();

    glEnable(GL_DEPTH_TEST);

    // FPS variables
    float lastFpsShowedTime = -2.f;
    int lastRenderedFrame = -1;
    int fpsFrames = 0;
    const float frameTime = 1.f / FPS_LIMIT;
    float deltaTime = 0.0f;
    float lastTime = static_cast<float>(glfwGetTime());

    // render loop
    // -----------
    while (!glfwWindowShouldClose(m_Window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        Time::SetDeltaTime(deltaTime);
        lastTime = currentTime;

        if (currentTime - lastFpsShowedTime > FPS_SHOWING_INTERVAL) {
            fps = static_cast<unsigned int>(fpsFrames / (currentTime - lastFpsShowedTime));
            Logger::Info("%d", fps);
            Time::SetCurrentFps(fps);
            lastFpsShowedTime = currentTime;
            fpsFrames = 0;
        }

        m_Input.Update();
        glfwPollEvents();
        processInput(m_Window);
        camera->Update(&m_Input, deltaTime);
        updateObjects(deltaTime);

        while (static_cast<int>(floor(static_cast<float>(glfwGetTime()) / frameTime)) == lastRenderedFrame) {
        }

        fpsFrames++;
        lastRenderedFrame = static_cast<int>(floor(static_cast<float>(glfwGetTime()) / frameTime));
        Render(SCR_WIDTH, SCR_HEIGHT);
    }

    glfwTerminate();
    return;
}


void Engine::updateObjects(float deltaTime) {
    for (int i = 0; i < m_Objects.size(); i++) {
        auto object = m_Objects[i];
        object->Update(deltaTime);
        if (object->animation) {
            object->animation->applyAnimations(object->transform, deltaTime);
        }
    }
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

    camera->SetScreenSize({static_cast<float>(scr_width), static_cast<float>(scr_height)});
    for (uint64_t i = 0; i < m_Objects.size(); i++) {
        auto object = m_Objects[i];
        if (!object->renderData || !object->transform)
            continue;

        auto data = object->renderData;
        auto transform = object->transform;

        auto model = data->model;

        float timeValue = glfwGetTime();

        ShaderProgram* shader = model->shader;
        shader->Use();

        Mat4 view = camera->GetViewMatrix();
        Vec3 viewPos = camera->GetPosition();

        Mat4 projection = camera->GetProjectionMatrix();

        // send matrix transform to shader
        shader->SetMat4("model", transform->GetTransformMatrix());
        shader->SetMat4("view", view);
        shader->SetVec3("viewPos", viewPos);

        // send material to shaders
        shader->SetFloat("material.shininess", data->material.shininess);
        // send light to shaders
        // pointLight
        char str[100];
        for (int i = 0; i < m_PointLights.size(); i++) {
            snprintf(str, sizeof(str), "pointLights[%d].position", i);
            shader->SetVec3(str, m_PointLights[i]->position);
            snprintf(str, sizeof(str), "pointLights[%d].ambient", i);
            shader->SetVec3(str, m_PointLights[i]->ambient);
            snprintf(str, sizeof(str), "pointLights[%d].diffuse", i);
            shader->SetVec3(str, m_PointLights[i]->diffuse);
            snprintf(str, sizeof(str), "pointLights[%d].specular", i);
            shader->SetVec3(str, m_PointLights[i]->specular);
            snprintf(str, sizeof(str), "pointLights[%d].linearDistCoeff", i);
            shader->SetFloat(str, m_PointLights[i]->linearDistCoeff);
            snprintf(str, sizeof(str), "pointLights[%d].quadraticDistCoeff", i);
            shader->SetFloat(str, m_PointLights[i]->quadraticDistCoeff);
            snprintf(str, sizeof(str), "pointLights[%d].constDistCoeff", i);
            shader->SetFloat(str, m_PointLights[i]->constDistCoeff);
        }

        shader->SetInt("lenArrPointL", m_PointLights.size());
        // directionLight
        for (int i = 0; i < m_DirLights.size(); i++) {
            snprintf(str, sizeof(str), "dirLight[%d].ambinet", i);
            shader->SetVec3(str, m_DirLights[0]->ambient);
            snprintf(str, sizeof(str), "dirLight[%d].specular", i);
            shader->SetVec3(str, m_DirLights[0]->specular);
            snprintf(str, sizeof(str), "dirLight[%d].direction", i);
            shader->SetVec3(str, m_DirLights[0]->direction);
            snprintf(str, sizeof(str), "dirLight[%d].diffuse", i);
            shader->SetVec3(str, m_DirLights[0]->diffuse);
        }
        shader->SetInt("lenArrDirL", m_DirLights.size());
        // spotLight
        for (int i = 0; i < m_SpotLights.size(); i++) {
            snprintf(str, sizeof(str), "spotLight[%d].diffuse", i);
            shader->SetVec3(str, m_SpotLights[i]->diffuse);
            snprintf(str, sizeof(str), "spotLight[%d].direction", i);
            shader->SetVec3(str, camera->GetFront());
            snprintf(str, sizeof(str), "spotLight[%d].ambient", i);
            shader->SetVec3(str, m_SpotLights[i]->ambient);
            snprintf(str, sizeof(str), "spotLight[%d].position", i);
            shader->SetVec3(str, camera->GetPosition());
            snprintf(str, sizeof(str), "spotLight[%d].specular", i);
            shader->SetVec3(str, m_SpotLights[i]->specular);
            snprintf(str, sizeof(str), "spotLight[%d].cutOff", i);
            shader->SetFloat(str, m_SpotLights[i]->cutOff);
            snprintf(str, sizeof(str), "spotLight[%d].linearDistCoeff", i);
            shader->SetFloat(str, m_SpotLights[i]->linearDistCoeff);
            snprintf(str, sizeof(str), "spotLight[%d].outerCutOff", i);
            shader->SetFloat(str, m_SpotLights[i]->outerCutOff);
            snprintf(str, sizeof(str), "spotLight[%d].constDistCoeff", i);
            shader->SetFloat(str, m_SpotLights[i]->constDistCoeff);
            snprintf(str, sizeof(str), "spotLight[%d].quadraticDistCoeff", i);
            shader->SetFloat(str, m_SpotLights[i]->quadraticDistCoeff);
        }
        shader->SetInt("lenArrSpotL", m_SpotLights.size());
        // send inf about texture
        data->material.texture.bind();
        shader->SetInt("material.duffuse", 0);
        shader->SetInt("material.specular", 1);
        // Note: currently we set the projection matrix each frame,
        // but since the projection matrix rarely changes it's
        // often best practice to set it outside the main loop only once.
        shader->SetMat4("projection", projection);

        glBindVertexArray(data->VAO);

        glDrawElements(GL_TRIANGLES, model->getLenIndices(), GL_UNSIGNED_INT, 0);
    }

    // Text and Sounds rendering
    for (uint64_t i = 0; i < m_Objects.size(); i++) {
        auto object = m_Objects[i];
        if (object->text) {
            object->text->RenderText();
        }

        if (object->sound && object->transform) {
            object->sound->SetPosition(object->transform->GetTranslation());
        }
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
    if (key > 0 && key < MAX_VALID_KEY && action == GLFW_PRESS) {
        s_Engine->m_Input.ButtonPress(key);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        s_Engine->m_Input.ButtonPress(button);
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
