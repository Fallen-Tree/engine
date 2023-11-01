#include "engine.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#include "camera.hpp"
#include "shader_loader.hpp"
#include "light.hpp"
#include "material.hpp"
#include "input.hpp"
#include "model.hpp"

// should send to all constants
const int maxValidKey = 350;

static Engine *s_Engine = nullptr;
EnvLight envL;

static Input *s_Input = nullptr;


float deltaTime = 0.0f;
float lastFrame = 0.0f;

Engine::Engine() {
    m_objects = std::vector<Object *>();
    m_Camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    s_Engine = this;
}

Engine::~Engine() {
    std::cout << "Goodbye";
}

void Engine::AddObject(Object *a) {
    m_objects.push_back(a);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const char *cubeSource = "/cube.obj";
const char *catSource = "/cat.obj";
const char *benchSource = "/bench.obj";

const char *vertexShaderSource = "/vertex/standart.vshader";
const char *fragmentShaderSource = "/fragment/standart.fshader";

void Engine::Run(int SCR_WIDTH, int SCR_HEIGHT) {
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
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);

    ShaderProgram shaderProgram = ShaderProgram(vShader, fShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    envL.m_Ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    envL.m_Diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    envL.m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
    envL.m_Position = glm::vec3(-0.2, -0.5, -1.2);

    // init a model
    Model * cubeModel = Model::loadFromFile(cubeSource);
    cubeModel->shader = shaderProgram;
    // transformation stores information about angle, scale, rotate and tranlsation.
    // Method makeTransform make mat4 transform(public var), after we send it to shaders.
    ModelInstance * cubeInstance = new ModelInstance(cubeModel, glm::vec3(0.f, 0.f, -3.f),
                                                                 glm::vec3(1.f, 1.f, 1.f),
                                                                 glm::mat4(1.0));
    Model * catModel = Model::loadFromFile(catSource);
    catModel->shader = shaderProgram;
    // transformation stores information about angle, scale, rotate and tranlsation.
    // Method makeTransform make mat4 transform(public var), after we send it to shaders.
    ModelInstance * catInstance = new ModelInstance(catModel, glm::vec3(0.f, 0.f, -3.f),
                                                                 glm::vec3(0.1f, 0.1f, 0.1f),
                                                                 glm::mat4(1.0));
    Model * benchModel = Model::loadFromFile(benchSource);
    benchModel->shader = shaderProgram;
    // transformation stores information about angle, scale, rotate and tranlsation.
    // Method makeTransform make mat4 transform(public var), after we send it to shaders.
    ModelInstance * benchInstance = new ModelInstance(benchModel, glm::vec3(0.f, 0.f, -3.f),
                                                                 glm::vec3(0.2f, 0.2f, 0.2f),
                                                                 glm::mat4(1.0));

    ModelInstance * modelInstance = catInstance;

    modelInstance->m_Mat.m_Ambient = glm::vec3(0.2, 0.1, 0.2);
    modelInstance->m_Mat.m_Diffuse = glm::vec3(0.7, 0.6, 0.7);
    modelInstance->m_Mat.m_Specular = glm::vec3(0.6, 0.7, 0.6);
    modelInstance->m_Mat.Shininess = 0.6;


    glGenVertexArrays(1, &modelInstance->GetModel()->VAO);
    glGenBuffers(1, &modelInstance->GetModel()->VBO);
    glGenBuffers(1, &modelInstance->GetModel()->EBO);


    // bind the Vertex Array Object first,
    // then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).

    glBindVertexArray(modelInstance->GetModel()->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, modelInstance->GetModel()->VBO);
    glBufferData(GL_ARRAY_BUFFER,
        modelInstance->GetModel()->getLenArrPoints() * sizeof(float),
        modelInstance->GetModel()->getPoints(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelInstance->GetModel()->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        modelInstance->GetModel()->getLenIndices() * sizeof(unsigned int),
        modelInstance->GetModel()->getIndices(), GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

    auto testObj = new Object();
    testObj->m_modelInstance = modelInstance;
    AddObject(testObj);
    glEnable(GL_DEPTH_TEST);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(m_Window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        m_Input.Update();
        glfwPollEvents();
        processInput(m_Window);


        Render(SCR_WIDTH, SCR_HEIGHT);
        m_Camera.Update(&m_Input, deltaTime);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &modelInstance->GetModel()->VAO);
    glDeleteBuffers(1, &modelInstance->GetModel()->VBO);
    glDeleteBuffers(1, &modelInstance->GetModel()->EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return;
}

void Engine::Render(int width, int height) {
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (uint64_t i = 0; i < m_objects.size(); i++) {
        auto object = m_objects[i];
        if (!object->m_modelInstance)
            continue;
        auto instance = object->m_modelInstance;

        auto model = instance->GetModel();

        float timeValue = glfwGetTime();
        instance->GetTransform()->Rotate(timeValue / 10000.0, glm::vec3(0.f, 0.f, 1.f));
        instance->GetTransform()->Rotate(timeValue / 10000.0, glm::vec3(0.f, 1.f, 0.f));

        ShaderProgram shader = model->shader;
        // draw our first triangle
        shader.Use();

        glm::mat4 view = m_Camera.GetViewMatrix();
        glm::vec3 viewPos = m_Camera.GetPosition();

        glm::mat4 projection = glm::perspective(
                                        glm::radians(m_Camera.GetZoom()),
                                        static_cast<float>(width) / static_cast<float>(height),
                                        0.1f, 100.0f);

        GLint objectColorLoc = shader.UniformLocation("objectColor");
        GLint modelLoc = shader.UniformLocation("model");
        GLint viewLoc = shader.UniformLocation("view");
        GLint viewPosLoc = shader.UniformLocation("viewPos");
        GLint projLoc = shader.UniformLocation("projection");
        // loc for material
        GLint ambientLoc =  shader.UniformLocation("material.ambient");
        GLint diffuseLoc =  shader.UniformLocation("material.diffuse");
        GLint specularLoc =  shader.UniformLocation("material.specular");
        GLint shininessLoc =  shader.UniformLocation("material.shininess");
        // loc for light
        GLint lightPositionLoc =  shader.UniformLocation("light.position");
        GLint lightAmbientLoc = shader.UniformLocation("light.ambient");
        GLint lightSpecularLoc = shader.UniformLocation("light.specualar");
        GLint lightDiffuseLoc = shader.UniformLocation("light.diffuse");


        instance->GetTransform()->Translate(glm::vec3(0.f, 0.f, -0.001f));


        // send color to shader
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(1., 1., 1.)));
        // send matrix transform to shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
            glm::value_ptr(instance->GetTransform()->GetTransformMatrix()));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
        // send material to shaders
        glUniform3fv(ambientLoc, 1, glm::value_ptr(instance->m_Mat.m_Ambient));
        glUniform3fv(diffuseLoc, 1, glm::value_ptr(instance->m_Mat.m_Diffuse));
        glUniform3fv(specularLoc, 1, glm::value_ptr(instance->m_Mat.m_Specular));
        glUniform1f(shininessLoc, instance->m_Mat.Shininess);
        // send light to shaders
        glUniform3fv(lightPositionLoc, 1, glm::value_ptr(envL.m_Position));
        glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(envL.m_Ambient));
        glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(envL.m_Specular));
        glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(envL.m_Diffuse));



        // Note: currently we set the projection matrix each frame,
        // but since the projection matrix rarely changes it's
        // often best practice to set it outside the main loop only once.
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(model->VAO);

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
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    s_Engine->m_Input.SetScrollOffset(static_cast<float>(yoffset));
}
