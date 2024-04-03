#include "engine.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <algorithm>

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
#include "rigid_body.hpp"
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
    m_Models = ComponentArray<Model>();
    m_Animations = ComponentArray<Animation>();
    m_Colliders = ComponentArray<Collider>();
    m_RigidBodies = ComponentArray<RigidBody>();
    m_Images = ComponentArray<Image>();
    m_Texts = ComponentArray<Text>();
    m_SkeletalAnimationsManagers = ComponentArray<SkeletalAnimationsManager>();

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
    for (auto &model : m_Models) {
        for (auto mesh : model.meshes) {
            glDeleteVertexArrays(1, &mesh.VAO);
            glDeleteBuffers(1, &mesh.VBO);
            glDeleteBuffers(1, &mesh.EBO);
        }
    }
}

// TODO(theblek): reuse object ids
Object Engine::NewObject() {
    ObjectHandle handle = m_ObjectCount++;
    Logger::Info("Created object %d", handle);
    return Object(this, handle);
}

void Engine::RemoveObject(ObjectHandle handle) {
    if (m_Transforms.HasData(handle))
        m_Transforms.RemoveData(handle);
    if (m_Models.HasData(handle))
        m_Models.RemoveData(handle);
    if (m_Colliders.HasData(handle))
        m_Colliders.RemoveData(handle);
    if (m_RigidBodies.HasData(handle))
        m_RigidBodies.RemoveData(handle);
    if (m_Texts.HasData(handle))
        m_Texts.RemoveData(handle);
    if (m_Images.HasData(handle))
        m_Images.RemoveData(handle);
    if (m_Animations.HasData(handle))
        m_Animations.RemoveData(handle);
    if (m_SpotLights.HasData(handle))
        m_SpotLights.RemoveData(handle);
    if (m_PointLights.HasData(handle))
        m_PointLights.RemoveData(handle);
    if (m_DirLights.HasData(handle))
        m_DirLights.RemoveData(handle);
    if (m_Behaviours.HasData(handle))
        m_Behaviours.RemoveData(handle);

    if (m_Parents.HasData(handle)) {
        auto parent = m_Parents.GetData(handle);
        auto &children = m_Children.GetData(parent);
        children.erase(std::find(children.begin(), children.end(), parent));
        m_Parents.RemoveData(handle);
    }
    if (m_Children.HasData(handle)) {
        for (auto child : m_Children.GetData(handle))
            RemoveObject(child);
        m_Children.RemoveData(handle);
    }
}

void Engine::AddChild(ObjectHandle parent, ObjectHandle child) {
    // TODO(theblek): Check for cycles in the tree
    m_Parents.SetData(child, parent);
    if (!m_Children.HasData(parent))
        m_Children.SetData(parent, std::vector<ObjectHandle>());
    m_Children.GetData(parent).push_back(child);
}

Object Engine::GetParent(ObjectHandle node) {
    return m_Parents.HasData(node) ? Object(this, m_Parents.GetData(node)) : Object();
}

Transform *Engine::GetTransform(ObjectHandle handle) {
    return m_Transforms.HasData(handle) ? &m_Transforms.GetData(handle) : nullptr;
}

Model *Engine::GetModel(ObjectHandle handle) {
    return m_Models.HasData(handle) ? &m_Models.GetData(handle) : nullptr;
}

Transform Engine::GetGlobalTransform(ObjectHandle handle) {
    auto transform = GetTransform(handle);
    if (!transform) {
        Logger::Error("Failed to get global transform: No transform on object");
        return Transform();
    }

    Mat4 modelMat = transform->GetTransformMatrix();
    auto cur = Object(this, handle);
    Vec3 scale = transform->GetScale();
    while (cur.GetParent().IsValid()) {
        auto pTransform = cur.GetParent().GetTransform();
        if (!pTransform) break;
        cur = cur.GetParent();
        modelMat = pTransform->GetTransformMatrix() * modelMat;
        scale *= pTransform->GetScale();
    }
    Transform result;
    result.SetScale(scale);
    result.SetTranslation(Vec3{modelMat[3][0], modelMat[3][1], modelMat[3][2]});
    modelMat[3][0] = modelMat[3][1] = modelMat[3][2] = 0;
    modelMat[3][3] = 1;
    result.SetRotation(glm::scale(modelMat, Vec3{1/scale.x, 1/scale.y, 1/scale.z}));

    return result;
}

Collider *Engine::GetCollider(ObjectHandle handle) {
    return m_Colliders.HasData(handle) ? &m_Colliders.GetData(handle) : nullptr;
}

RigidBody *Engine::GetRigidBody(ObjectHandle handle) {
    return m_RigidBodies.HasData(handle) ? &m_RigidBodies.GetData(handle) : nullptr;
}

Animation *Engine::GetAnimation(ObjectHandle handle) {
    return m_Animations.HasData(handle) ? &m_Animations.GetData(handle) : nullptr;
}

Text *Engine::GetText(ObjectHandle handle) {
    return m_Texts.HasData(handle) ? &m_Texts.GetData(handle) : nullptr;
}

SkeletalAnimationsManager *Engine::GetSkeletalAnimationsManager(ObjectHandle handle) {
    return m_SkeletalAnimationsManagers.HasData(handle) ?
        &m_SkeletalAnimationsManagers.GetData(handle) : nullptr;
}

Image *Engine::GetImage(ObjectHandle handle) {
    return m_Images.HasData(handle) ? &m_Images.GetData(handle) : nullptr;
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

Model &Engine::AddModel(ObjectHandle id, Model v) {
    m_Models.SetData(id, v);
    return m_Models.GetData(id);
}

Collider &Engine::AddCollider(ObjectHandle id, Collider v) {
    m_Colliders.SetData(id, v);
    return m_Colliders.GetData(id);
}

RigidBody &Engine::AddRigidBody(ObjectHandle id, RigidBody v) {
    m_RigidBodies.SetData(id, v);
    return m_RigidBodies.GetData(id);
}

Text &Engine::AddText(ObjectHandle id, Text v) {
    m_Texts.SetData(id, v);
    return m_Texts.GetData(id);
}

SkeletalAnimationsManager &Engine::AddSkeletalAnimationsManager
    (ObjectHandle id, SkeletalAnimationsManager v) {
    m_SkeletalAnimationsManagers.SetData(id, v);
    return m_SkeletalAnimationsManagers.GetData(id);
}

Image &Engine::AddImage(ObjectHandle id, Image v) {
    m_Images.SetData(id, v);
    return m_Images.GetData(id);
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
            Logger::Info("FPS: %d", fps);
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
    // Find collisions on rigidbodies and handle them
    for (int i = 0; i < m_RigidBodies.GetSize(); i++) {
        auto handle = m_RigidBodies.GetFromInternal(i);
        if (!m_Colliders.HasData(handle) || !m_Transforms.HasData(handle)) {
            Logger::Error(
                "RigidBody on object %d must have a collider and a transform to work",
                handle);
            continue;
        }
        for (int j = i + 1; j < m_RigidBodies.GetSize(); j++) {
            auto handle2 = m_RigidBodies.GetFromInternal(j);
            if (!m_Colliders.HasData(handle2) || !m_Transforms.HasData(handle2)) {
                Logger::Error(
                    "RigidBody on object %d must have a collider and a transform to work",
                    handle2);
                continue;
            }

            auto c1 = m_Colliders.GetData(handle);
            auto c2 = m_Colliders.GetData(handle2);
            auto t1 = GetGlobalTransform(handle);
            auto t2 = GetGlobalTransform(handle2);
            if (c1.Collide(t1, &c2, t2)) {
                m_RigidBodies.GetData(handle).ResolveCollisions(
                        t1, t2, &c1, &c2,
                        &m_RigidBodies.GetData(handle2), deltaTime);
            }
        }
    }

    // Update Animations
    for (int i = 0; i < m_Animations.GetSize(); i++) {
        ObjectHandle handle = m_Animations.GetFromInternal(i);
        if (!m_Transforms.HasData(handle)) {
            Logger::Error(
                "Animation component on object %d requires transform component to work",
                handle);
            continue;
        }
        m_Animations.entries[i].applyAnimations(&m_Transforms.GetData(handle), deltaTime);
    }

    // Update Skeletal Animations
    for (int i = 0; i < m_SkeletalAnimationsManagers.GetSize(); i++) {
        m_SkeletalAnimationsManagers.entries[i].UpdateAnimation(deltaTime);
    }

    // Update RigidBodies
    for (int i = 0; i < m_RigidBodies.GetSize(); i++) {
        auto handle = m_RigidBodies.GetFromInternal(i);
        if (!m_Colliders.HasData(handle) || !m_Transforms.HasData(handle)) {
            Logger::Error(
                "RigidBody on object %d must have a collider and a transform to work",
                handle);
            continue;
        }
        m_RigidBodies.GetData(handle).Update(&m_Transforms.GetData(handle), deltaTime);
    }

    for (auto behaviour : m_Behaviours) {
        behaviour->Update(deltaTime);
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
    for (int i = 0; i < m_Models.GetSize(); i++) {
        ObjectHandle id = m_Models.GetFromInternal(i);
        if (!m_Transforms.HasData(id)) continue;

        auto model = m_Models.GetData(id);
        auto transform = GetGlobalTransform(id);
        Mat4 projection = camera->GetProjectionMatrix();

        for (RenderMesh mesh : model.meshes) {
            ShaderProgram* shader = model.shader;
            if (shader == nullptr) {
                Logger::Error("No shader connected with Mesh!");
            }
            shader->Use();

            Mat4 view = camera->GetViewMatrix();
            Vec3 viewPos = camera->GetPosition();

            // send matrix transform to shader
            shader->SetMat4("model", transform.GetTransformMatrix());
            shader->SetMat4("view", view);
            shader->SetVec3("viewPos", viewPos);

            if (m_SkeletalAnimationsManagers.HasData(id)) {
                auto bones = m_SkeletalAnimationsManagers.GetData(id).GetFinalBoneMatrices();
                for (int i = 0; i < bones.size(); ++i) {
                    shader->SetMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), bones[i]);
                }
            }

            // send material to shaders
            shader->SetFloat("material.shininess", mesh.material.shininess);
            // send light to shaders
            // pointLight
            char str[100];
            for (auto light : m_PointLights) {
                snprintf(str, sizeof(str), "pointLights[%d].position", i);
                shader->SetVec3(str, light.position);
                snprintf(str, sizeof(str), "pointLights[%d].ambient", i);
                shader->SetVec3(str, light.ambient);
                snprintf(str, sizeof(str), "pointLights[%d].diffuse", i);
                shader->SetVec3(str, light.diffuse);
                snprintf(str, sizeof(str), "pointLights[%d].specular", i);
                shader->SetVec3(str, light.specular);
                snprintf(str, sizeof(str), "pointLights[%d].linearDistCoeff", i);
                shader->SetFloat(str, light.linearDistCoeff);
                snprintf(str, sizeof(str), "pointLights[%d].quadraticDistCoeff", i);
                shader->SetFloat(str, light.quadraticDistCoeff);
                snprintf(str, sizeof(str), "pointLights[%d].constDistCoeff", i);
                shader->SetFloat(str, light.constDistCoeff);
            }

            shader->SetInt("lenArrPointL", m_PointLights.GetSize());
            // directionLight
            for (auto light : m_DirLights) {
                snprintf(str, sizeof(str), "dirLight[%d].ambinet", i);
                shader->SetVec3(str, light.ambient);
                snprintf(str, sizeof(str), "dirLight[%d].specular", i);
                shader->SetVec3(str, light.specular);
                snprintf(str, sizeof(str), "dirLight[%d].direction", i);
                shader->SetVec3(str, light.direction);
                snprintf(str, sizeof(str), "dirLight[%d].diffuse", i);
                shader->SetVec3(str, light.diffuse);
            }
            shader->SetInt("lenArrDirL", m_DirLights.GetSize());
            // spotLight
            for (auto light : m_SpotLights) {
                snprintf(str, sizeof(str), "spotLight[%d].diffuse", i);
                shader->SetVec3(str, light.diffuse);
                snprintf(str, sizeof(str), "spotLight[%d].direction", i);
                shader->SetVec3(str, camera->GetFront());
                snprintf(str, sizeof(str), "spotLight[%d].ambient", i);
                shader->SetVec3(str, light.ambient);
                snprintf(str, sizeof(str), "spotLight[%d].position", i);
                shader->SetVec3(str, camera->GetPosition());
                snprintf(str, sizeof(str), "spotLight[%d].specular", i);
                shader->SetVec3(str, light.specular);
                snprintf(str, sizeof(str), "spotLight[%d].cutOff", i);
                shader->SetFloat(str, light.cutOff);
                snprintf(str, sizeof(str), "spotLight[%d].linearDistCoeff", i);
                shader->SetFloat(str, light.linearDistCoeff);
                snprintf(str, sizeof(str), "spotLight[%d].outerCutOff", i);
                shader->SetFloat(str, light.outerCutOff);
                snprintf(str, sizeof(str), "spotLight[%d].constDistCoeff", i);
                shader->SetFloat(str, light.constDistCoeff);
                snprintf(str, sizeof(str), "spotLight[%d].quadraticDistCoeff", i);
                shader->SetFloat(str, light.quadraticDistCoeff);
            }
            shader->SetInt("lenArrSpotL", m_SpotLights.GetSize());
            // send inf about texture
            mesh.material.texture.bind();
            shader->SetInt("material.duffuse", 0);
            shader->SetInt("material.specular", 1);
            // Note: currently we set the projection matrix each frame,
            // but since the projection matrix rarely changes it's
            // often best practice to set it outside the main loop only once.
            shader->SetMat4("projection", projection);

            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.getLenIndices(), GL_UNSIGNED_INT, 0);
        }
    }

    //      Image rendering
    for (auto &image : m_Images) {
        image.Render();
    }

    for (auto &text : m_Texts) {
        text.RenderText();
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
