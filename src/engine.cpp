#include "engine.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

Engine::Engine() {
    m_objects = std::vector<Object *>();
}

Engine::~Engine() {
    std::cout << "Goodbye";
}

void Engine::AddObject(Object *a) {
    m_objects.push_back(a);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 transform;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(aPos, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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
    m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (m_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

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

    std::vector<float> testVertices = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    std::vector<unsigned int> testIndices = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    // init a model
    Model * testModel = new Model(testVertices, testIndices);
    testModel->shader = shaderProgram;
    // transformation stores information about angle, scale, rotate and tranlsation.
    // Method makeTransform make mat4 transform(public var), after we send it to shaders.
    ModelInstance * modelInstance = new ModelInstance(glm::vec3(1.f, 1.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f), 0, glm::vec3(0.f, 0.f, 0.f), testModel);

    glGenVertexArrays(1, &modelInstance->getModel()->VAO);
    glGenBuffers(1, &modelInstance->getModel()->VBO);
    glGenBuffers(1, &modelInstance->getModel()->EBO);

    // bind the Vertex Array Object first,
    // then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(modelInstance->getModel()->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, modelInstance->getModel()->VBO);
    glBufferData(GL_ARRAY_BUFFER,
        modelInstance->getModel()->getLenArrPoints() * sizeof(float),
        modelInstance->getModel()->getPoints(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelInstance->getModel()->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        modelInstance->getModel()->getLenIndices() * sizeof(unsigned int),
        modelInstance->getModel()->getIndices(), GL_STATIC_DRAW);

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
    while (!glfwWindowShouldClose(m_window)) {
        // input
        // -----
        processInput(m_window);

        Render();
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &modelInstance->getModel()->VAO);
    glDeleteBuffers(1, &modelInstance->getModel()->VBO);
    glDeleteBuffers(1, &modelInstance->getModel()->EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return;
}

void Engine::Render() {
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < m_objects.size(); i++) {
        auto object = m_objects[i];
        if (!object->m_modelInstance)
            continue;
        auto instance = object->m_modelInstance;
        auto model = instance->getModel();

        float timeValue = glfwGetTime();
        instance->setRotation(glm::vec3(0.f, 0.f, 1.f), timeValue * 2);

        unsigned shader = model->shader;
        int transformLoc = glGetUniformLocation(shader, "transform");
        // draw our first triangle
        glUseProgram(shader);
        // send matrix transform to shader
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(instance->transform));

        glBindVertexArray(model->VAO);
        glDrawElements(GL_TRIANGLES, model->getLenIndices(), GL_UNSIGNED_INT, 0);
    }
    glfwSwapBuffers(m_window);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
