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
#include "animation.hpp"

int viewportWidth, viewportHeight;
// Left bottom corner coordinates of viewport
int viewportStartX, viewportStartY;
// For resoliton and initial window size. 1600x900 for example.
int scrWidth, scrHeight;


static Engine *s_Engine = nullptr;
static Input *s_Input = nullptr;

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
void processInput(GLFWwindow *window);

Engine::Engine(int SCR_WIDTH, int SCR_HEIGHT) {
    m_Objects = std::vector<Object *>();
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    s_Engine = this;
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
    m_Objects.push_back(a);
}



void Engine::Run(int SCR_WIDTH, int SCR_HEIGHT) {
    scrWidth = SCR_WIDTH;
    scrHeight = SCR_HEIGHT;
    viewportWidth = SCR_WIDTH;
    viewportHeight = SCR_HEIGHT;
    viewportStartX = 0;
    viewportStartY = 0;

    m_Input.SetWindow(m_Window);
    m_Input.SetMode(MODE, VALUE);
    m_Input.InitMouse();

    glEnable(GL_DEPTH_TEST);

    float lastFpsShowedTime = 0.f;
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
        lastTime = currentTime;

        if (currentTime - lastFpsShowedTime > FPS_SHOWING_INTERVAL) {
            Logger::Info("FPS: %d", static_cast<int>(fpsFrames / (currentTime - lastFpsShowedTime)));
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

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return;
}

void Engine::updateObjects(float deltaTime) {
    for (int i = 0; i < m_Objects.size(); i++) {
        auto object = m_Objects[i];
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

    for (uint64_t i = 0; i < m_Objects.size(); i++) {
        auto object = m_Objects[i];
        if (!object->renderData || !object->transform)
            continue;

        auto data = object->renderData;
        auto transform = object->transform;

        auto model = data->model;

        float timeValue = glfwGetTime();

        transform->Rotate(glm::radians(0.1f), glm::radians(0.1f), glm::radians(0.1f));

        ShaderProgram* shader = model->shader;
        // draw our first triangle
        shader->Use();

        glm::mat4 view = camera->GetViewMatrix();
        glm::vec3 viewPos = camera->GetPosition();

        glm::mat4 projection = glm::perspective(
                                        glm::radians(camera->GetZoom()),
                                        static_cast<float>(scr_width) / static_cast<float>(scr_height),
                                        0.1f, 100.0f);

      // send matrix transform to shader
        shader->SetMat4("model", transform->GetTransformMatrix());
        shader->SetMat4("view", view);
        shader->SetVec3("viewPos", viewPos);

        // send material to shaders
        shader->SetFloat("material.shininess", data->material.shininess);
        // send light to shaders
        // pointLight
        char str[100];
        for (int i = 0; i < pointLights.size(); i++) {
            snprintf(str, sizeof(str), "pointLights[%d].position", i);
            shader->SetVec3(str, pointLights[i].position);
            snprintf(str, sizeof(str), "pointLights[%d].ambient", i);
            shader->SetVec3(str, pointLights[i].ambient);
            snprintf(str, sizeof(str), "pointLights[%d].diffuse", i);
            shader->SetVec3(str, pointLights[i].diffuse);
            snprintf(str, sizeof(str), "pointLights[%d].specular", i);
            shader->SetVec3(str, pointLights[i].specular);
            snprintf(str, sizeof(str), "pointLights[%d].linearDistCoeff", i);
            shader->SetFloat(str, pointLights[i].linearDistCoeff);
            snprintf(str, sizeof(str), "pointLights[%d].quadraticDistCoeff", i);
            shader->SetFloat(str, pointLights[i].quadraticDistCoeff);
            snprintf(str, sizeof(str), "pointLights[%d].constDistCoeff", i);
            shader->SetFloat(str, pointLights[i].constDistCoeff);
        }
        shader->SetInt("lenArrPointL", pointLights.size());
        // directionLight
        shader->SetVec3("dirLight.ambient", dirLight.ambient);
        shader->SetVec3("dirLight.specular", dirLight.specular);
        shader->SetVec3("dirLight.direction", dirLight.direction);
        shader->SetVec3("dirLight.diffuse", dirLight.diffuse);
        // spotLight
        for (int i = 0; i < spotLights.size(); i++) {
            snprintf(str, sizeof(str), "spotLight[%d].diffuse", i);
            shader->SetVec3(str, spotLights[i].diffuse);
            snprintf(str, sizeof(str), "spotLight[%d].direction", i);
            shader->SetVec3(str, camera->GetFront());
            snprintf(str, sizeof(str), "spotLight[%d].ambient", i);
            shader->SetVec3(str, spotLights[i].ambient);
            snprintf(str, sizeof(str), "spotLight[%d].position", i);
            shader->SetVec3(str, camera->GetPosition());
            snprintf(str, sizeof(str), "spotLight[%d].specular", i);
            shader->SetVec3(str, spotLights[i].specular);
            snprintf(str, sizeof(str), "spotLight[%d].cutOff", i);
            shader->SetFloat(str, spotLights[i].cutOff);
            snprintf(str, sizeof(str), "spotLight[%d].linearDistCoeff", i);
            shader->SetFloat(str, spotLights[i].linearDistCoeff);
            snprintf(str, sizeof(str), "spotLight[%d].outerCutOff", i);
            shader->SetFloat(str, spotLights[i].outerCutOff);
            snprintf(str, sizeof(str), "spotLight[%d].constDistCoeff", i);
            shader->SetFloat(str, spotLights[i].constDistCoeff);
            snprintf(str, sizeof(str), "spotLight[%d].quadraticDistCoeff", i);
            shader->SetFloat(str, spotLights[i].quadraticDistCoeff);
        }
        shader->SetInt("lenArrSpotL", spotLights.size());
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
