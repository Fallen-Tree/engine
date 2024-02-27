#include "engine.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#include "engine_config.hpp"
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
#include "behaviour.hpp"
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
    camera = new Camera(Vec3(0.0f, 0.0f, 3.0f));
    s_Engine = this;
    // Pre-allocate all needed memory
    m_Transforms = ComponentArray<Transform>();
    m_RenderDatas = ComponentArray<RenderData>();
    m_Animations = ComponentArray<Animation>();
    m_Colliders = ComponentArray<Collider>();
    m_Texts = ComponentArray<Text>();

    m_PointLights = ComponentArray<PointLight>();
    m_DirLights = ComponentArray<DirLight>();
    m_SpotLights = ComponentArray<SpotLight>();
    m_ObjectCount = 0;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    for (int i = 0; i < m_RenderDatas.GetSize(); i++) {
        glDeleteVertexArrays(1, &m_RenderDatas.entries[i].VAO);
        glDeleteBuffers(1, &m_RenderDatas.entries[i].VBO);
        glDeleteBuffers(1, &m_RenderDatas.entries[i].EBO);
    }
}

Object Engine::NewObject() {
    return Object(*this, m_ObjectCount++);
}

Transform *Engine::GetTransform(ObjectHandle handle) {
    return m_Transforms.HasData(handle) ? &m_Transforms.GetData(handle) : nullptr;
}

RenderData *Engine::GetRenderData(ObjectHandle handle) {
    return m_RenderDatas.HasData(handle) ? &m_RenderDatas.GetData(handle) : nullptr;
}

Collider *Engine::GetCollider(ObjectHandle handle) {
    return m_Colliders.HasData(handle) ? &m_Colliders.GetData(handle) : nullptr;
}

Animation *Engine::GetAnimation(ObjectHandle handle) {
    return m_Animations.HasData(handle) ? &m_Animations.GetData(handle) : nullptr;
}

Text *Engine::GetText(ObjectHandle handle) {
    return m_Texts.HasData(handle) ? &m_Texts.GetData(handle) : nullptr;
}

PointLight *Engine::GetPointLight(ObjectHandle handle) {
    return m_PointLights.HasData(handle) ? &m_PointLights.GetData(handle) : nullptr;
}

SpotLight *Engine::GetSpotLight(ObjectHandle handle) {
    return m_SpotLights.HasData(handle) ? &m_SpotLights.GetData(handle) : nullptr;
}

DirLight *Engine::GetDirLight(ObjectHandle handle) {
    return m_DirLights.HasData(handle) ? &m_DirLights.GetData(handle) : nullptr;
}

Behaviour *Engine::GetBehaviour(ObjectHandle handle) {
    return m_Behaviours.HasData(handle) ? m_Behaviours.GetData(handle) : nullptr;
}

Transform &Engine::AddTransform(ObjectHandle id, Transform v) {
    m_Transforms.SetData(id, v);
    return m_Transforms.GetData(id);
}

RenderData &Engine::AddRenderData(ObjectHandle id, RenderData v) {
    m_RenderDatas.SetData(id, v);
    return m_RenderDatas.GetData(id);
}

Collider &Engine::AddCollider(ObjectHandle id, Collider v) {
    m_Colliders.SetData(id, v);
    return m_Colliders.GetData(id);
}


Text &Engine::AddText(ObjectHandle id, Text v) {
    m_Texts.SetData(id, v);
    return m_Texts.GetData(id);
}

Animation &Engine::AddAnimation(ObjectHandle id, Animation v) {
    m_Animations.SetData(id, v);
    return m_Animations.GetData(id);
}

PointLight &Engine::AddPointLight(ObjectHandle id, PointLight v) {
    m_PointLights.SetData(id, v);
    return m_PointLights.GetData(id);
}

SpotLight &Engine::AddSpotLight(ObjectHandle id, SpotLight v) {
    m_SpotLights.SetData(id, v);
    return m_SpotLights.GetData(id);
}

DirLight &Engine::AddDirLight(ObjectHandle id, DirLight v) {
    m_DirLights.SetData(id, v);
    return m_DirLights.GetData(id);
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
    float lastTime = 0.0f;


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
        Render(viewportWidth, viewportHeight);
    }

    glfwTerminate();
    return;
}


void Engine::updateObjects(float deltaTime) {
    for (int i = 0; i < m_Behaviours.GetSize(); i++) {
        m_Behaviours.entries[i]->Update(deltaTime);
    }
    for (int i = 0; i < m_Animations.GetSize(); i++) {
        ObjectHandle id = m_Animations.GetFromInternal(i);
        if (!m_Transforms.HasData(id)) {
            Logger::Warn("Animation component on object %d requires transform component to work");
        }
        m_Animations.entries[i].applyAnimations(&m_Transforms.GetData(id), deltaTime);
    }
}

void Engine::Render(int scr_width, int scr_height) {
    // Coloring all window (black)
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Coloring only viewport (sky)
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewportStartX, viewportStartY, viewportWidth, viewportHeight);
    glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    camera->SetScreenSize(
        {
            static_cast<float>(viewportWidth),
            static_cast<float>(viewportHeight)
        });
    for (int i = 0; i < m_RenderDatas.GetSize(); i++) {
        ObjectHandle id = m_RenderDatas.GetFromInternal(i);
        if (!m_Transforms.HasData(id)) continue;

        auto data = m_RenderDatas.GetData(i);
        auto transform = m_Transforms.GetData(id);

        auto model = data.model;

        ShaderProgram* shader = model->shader;
        shader->Use();

        Mat4 view = camera->GetViewMatrix();
        Vec3 viewPos = camera->GetPosition();

        Mat4 projection = camera->GetProjectionMatrix();

        // send matrix transform to shader
        shader->SetMat4("model", transform.GetTransformMatrix());
        shader->SetMat4("view", view);
        shader->SetVec3("viewPos", viewPos);

        // send material to shaders
        shader->SetFloat("material.shininess", data.material.shininess);
        // send light to shaders
        // pointLight
        char str[100];
        for (int i = 0; i < m_PointLights.GetSize(); i++) {
            snprintf(str, sizeof(str), "pointLights[%d].position", i);
            shader->SetVec3(str, m_PointLights.entries[i].position);
            snprintf(str, sizeof(str), "pointLights[%d].ambient", i);
            shader->SetVec3(str, m_PointLights.entries[i].ambient);
            snprintf(str, sizeof(str), "pointLights[%d].diffuse", i);
            shader->SetVec3(str, m_PointLights.entries[i].diffuse);
            snprintf(str, sizeof(str), "pointLights[%d].specular", i);
            shader->SetVec3(str, m_PointLights.entries[i].specular);
            snprintf(str, sizeof(str), "pointLights[%d].linearDistCoeff", i);
            shader->SetFloat(str, m_PointLights.entries[i].linearDistCoeff);
            snprintf(str, sizeof(str), "pointLights[%d].quadraticDistCoeff", i);
            shader->SetFloat(str, m_PointLights.entries[i].quadraticDistCoeff);
            snprintf(str, sizeof(str), "pointLights[%d].constDistCoeff", i);
            shader->SetFloat(str, m_PointLights.entries[i].constDistCoeff);
        }

        shader->SetInt("lenArrPointL", m_PointLights.GetSize());
        // directionLight
        for (int i = 0; i < m_DirLights.GetSize(); i++) {
            snprintf(str, sizeof(str), "dirLight[%d].ambinet", i);
            shader->SetVec3(str, m_DirLights.entries[i].ambient);
            snprintf(str, sizeof(str), "dirLight[%d].specular", i);
            shader->SetVec3(str, m_DirLights.entries[i].specular);
            snprintf(str, sizeof(str), "dirLight[%d].direction", i);
            shader->SetVec3(str, m_DirLights.entries[i].direction);
            snprintf(str, sizeof(str), "dirLight[%d].diffuse", i);
            shader->SetVec3(str, m_DirLights.entries[i].diffuse);
        }
        shader->SetInt("lenArrDirL", m_DirLights.GetSize());
        // spotLight
        for (int i = 0; i < m_SpotLights.GetSize(); i++) {
            snprintf(str, sizeof(str), "spotLight[%d].diffuse", i);
            shader->SetVec3(str, m_SpotLights.entries[i].diffuse);
            snprintf(str, sizeof(str), "spotLight[%d].direction", i);
            shader->SetVec3(str, camera->GetFront());
            snprintf(str, sizeof(str), "spotLight[%d].ambient", i);
            shader->SetVec3(str, m_SpotLights.entries[i].ambient);
            snprintf(str, sizeof(str), "spotLight[%d].position", i);
            shader->SetVec3(str, camera->GetPosition());
            snprintf(str, sizeof(str), "spotLight[%d].specular", i);
            shader->SetVec3(str, m_SpotLights.entries[i].specular);
            snprintf(str, sizeof(str), "spotLight[%d].cutOff", i);
            shader->SetFloat(str, m_SpotLights.entries[i].cutOff);
            snprintf(str, sizeof(str), "spotLight[%d].linearDistCoeff", i);
            shader->SetFloat(str, m_SpotLights.entries[i].linearDistCoeff);
            snprintf(str, sizeof(str), "spotLight[%d].outerCutOff", i);
            shader->SetFloat(str, m_SpotLights.entries[i].outerCutOff);
            snprintf(str, sizeof(str), "spotLight[%d].constDistCoeff", i);
            shader->SetFloat(str, m_SpotLights.entries[i].constDistCoeff);
            snprintf(str, sizeof(str), "spotLight[%d].quadraticDistCoeff", i);
            shader->SetFloat(str, m_SpotLights.entries[i].quadraticDistCoeff);
        }
        shader->SetInt("lenArrSpotL", m_SpotLights.GetSize());
        // send inf about texture
        data.material.texture.bind();
        shader->SetInt("material.duffuse", 0);
        shader->SetInt("material.specular", 1);
        // Note: currently we set the projection matrix each frame,
        // but since the projection matrix rarely changes it's
        // often best practice to set it outside the main loop only once.
        shader->SetMat4("projection", projection);

        glBindVertexArray(data.VAO);

        glDrawElements(GL_TRIANGLES, model->getLenIndices(), GL_UNSIGNED_INT, 0);
    }

    for (int i = 0; i < m_Texts.GetSize(); i++) {
        m_Texts.entries[i].RenderText();
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
