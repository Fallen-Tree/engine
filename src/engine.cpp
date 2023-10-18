#include "engine.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "camera.hpp"

static Engine *s_Engine = nullptr;

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

float lastX = 0;
float lastY = 0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(position, 1.0f);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.7f, 1.0f);\n"
    "}\n\0";

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
    glfwSetCursorPosCallback(m_Window, mouse_callback);
    glfwSetScrollCallback(m_Window, scroll_callback);

    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    std::vector<GLuint> cubeIndices {
        2, 6, 7,
        2, 3, 7,

        0, 4, 5,
        0, 1, 5,

        0, 2, 6,
        0, 4, 6,

        1, 3, 7,
        1, 5, 7,

        0, 2, 3,
        0, 1, 3,

        4, 6, 7,
        4, 5, 7
    };

    std::vector<GLfloat> cubeVertices {
        -1, -1,  1,
         1, -1,  1,
        -1,  1,  1,
         1,  1,  1,
        -1, -1, -1,
         1, -1, -1,
        -1,  1, -1,
         1,  1, -1
    };

    // init a model
    Model * testModel = new Model(cubeVertices, cubeIndices);
    testModel->shader = shaderProgram;
    // transformation stores information about angle, scale, rotate and tranlsation.
    // Method makeTransform make mat4 transform(public var), after we send it to shaders.
    ModelInstance * modelInstance = new ModelInstance(testModel, glm::vec3(0.f, 0.f, -3.f), glm::vec3(1.f, 1.f, 1.f), glm::mat4(1.0));

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

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

    // render loop
    // -----------
    while (!glfwWindowShouldClose(m_Window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(m_Window);

        Render(SCR_WIDTH, SCR_HEIGHT);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &modelInstance->GetModel()->VAO);
    glDeleteBuffers(1, &modelInstance->GetModel()->VBO);
    glDeleteBuffers(1, &modelInstance->GetModel()->EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return;
}

void Engine::Render(int width, int height) {
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (long long unsigned int i = 0; i < m_objects.size(); i++) {
        auto object = m_objects[i];
        if (!object->m_modelInstance)
            continue;
        auto instance = object->m_modelInstance;
        auto model = instance->GetModel();

        float timeValue = glfwGetTime();
        instance->GetTransform()->ModifyRotate(timeValue / 10000.0, glm::vec3(0.f, 0.f, 1.f));
        instance->GetTransform()->ModifyRotate(timeValue / 10000.0, glm::vec3(0.f, 1.f, 0.f));
        
        unsigned shader = model->shader;
        // draw our first triangle
        glUseProgram(shader);

        glm::mat4 view = m_Camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(
                                        glm::radians(m_Camera.GetZoom()),
                                        static_cast<float>(width) / static_cast<float>(height),
                                        0.1f, 100.0f);

        GLint modelLoc = glGetUniformLocation(shader, "model");
        GLint viewLoc = glGetUniformLocation(shader, "view");
        GLint projLoc = glGetUniformLocation(shader, "projection");

        instance->GetTransform()->ModifyTranslate(glm::vec3(0.f, 0.f, -0.001f));

        // send matrix transform to shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(instance->GetTransform()->GetTransformMatrix()));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        s_Engine->m_Camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        s_Engine->m_Camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        s_Engine->m_Camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        s_Engine->m_Camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    s_Engine->m_Camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    s_Engine->m_Camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
