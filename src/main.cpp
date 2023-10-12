#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "model/model.hpp"
#include "model/modelInstance.hpp"
#include "model/material.hpp"
#include "model/light.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;"
    "out vec3 Normal;"
    "out vec3 FragPos;"
    "uniform mat4 transform;"
    "void main()\n"
    "{\n"
    "gl_Position = transform * vec4(aPos, 1.0);\n"
    "Normal = mat3(transpose(inverse(transform))) * aNormal;"
    "FragPos = vec3(transform * vec4(aPos, 1.0));"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "in vec3 Normal;"
    "in vec3 FragPos;"
    "out vec4 FragColor;\n"
    "struct Material {"
    "vec3 ambient;"
    "vec3 diffuse;"
    "vec3 specular;"
    "float shininess;"
    "};"
    "struct Light {"
    "vec3 position;"
    "vec3 ambient;"
    "vec3 diffuse;"
    "vec3 specular;"
    "};"
    "uniform Light light;"
    "uniform Material material;"
    "uniform vec3 objectColor;"
    "uniform vec3 viewPos;"
    "void main()\n"
    "{\n"
    "vec3 ambient = light.ambient *  material.ambient;"
    "vec3 norm = normalize(Normal);"
    "vec3 lightDir = normalize(light.position - FragPos);"
    "float diff = max(dot(norm, lightDir), 0.0);"
    "vec3 diffuse = (diff + material.diffuse) * light.diffuse;"
    "float specularStrength = 0.5;"
    "vec3 viewDir = normalize(viewPos - FragPos);"
    "vec3 reflectDir = reflect(-lightDir, norm);"
    "float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);"
    "vec3 specular = light.specular * (spec  * material.specular);"
    "FragColor = vec4((diffuse + ambient + specular) * objectColor, 1.0f);\n"
    "}\n\0";

glm::vec3 viewPos(0.f, 0.f, 0.f);

int main() {
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
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

    Material mat;
    mat.m_Ambient = glm::vec3(0.2, 0.1, 0.2);
    mat.m_Diffuse = glm::vec3(0.7, 0.6, 0.7);
    mat.m_Specular = glm::vec3(0.6, 0.7, 0.6);
    mat.Shininess = 0.6;

    EnvLight envL;
    envL.m_Ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    envL.m_Diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    envL.m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
    envL.m_Position = glm::vec3(-0.2, -0.5, -1.2);

    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    Model * cube = new Model(vertices);
    ModelInstance * instCube = new ModelInstance(
        glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), 0, glm::vec3(0.f, 0.f, 0.f), cube);
    glGenVertexArrays(1, &instCube->getModel()->VAO);
    glGenBuffers(1, &instCube->getModel()->VBO);

    glBindBuffer(GL_ARRAY_BUFFER, instCube->getModel()->VBO);
    glBufferData(GL_ARRAY_BUFFER, instCube->getModel()->getLenArrPoints() * sizeof(float),
        instCube->getModel()->getPoints(), GL_STATIC_DRAW);

    glBindVertexArray(instCube->getModel()->VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    glEnable(GL_DEPTH_TEST);
    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // get time for demonstration
        // float timeValue = glfwGetTime();
        // modelInstance->setRotation(glm::vec3(0.f, 0.f, 1.f), timeValue * 2);
        // find location of mat4 tranform
        int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
        // loc for material
        int ambientLoc =  glGetUniformLocation(shaderProgram, "material.ambient");
        int diffuseLoc =  glGetUniformLocation(shaderProgram, "material.diffuse");
        int specularLoc =  glGetUniformLocation(shaderProgram, "material.specular");
        int shininessLoc =  glGetUniformLocation(shaderProgram, "material.shininess");
        // loc for light
        int lightPositionLoc =  glGetUniformLocation(shaderProgram, "light.position");
        int lightAmbientLoc = glGetUniformLocation(shaderProgram, "light.ambient");
        int lightSpecularLoc = glGetUniformLocation(shaderProgram, "light.specualar");
        int lightDiffuseLoc = glGetUniformLocation(shaderProgram, "light.diffuse");

        // draw our first triangle

        // send matrix transform to shader
        // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(modelInstance->transform));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(0.53, 0.43, 0.23)));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
        // send material to shaders
        glUniform3fv(ambientLoc, 1, glm::value_ptr(mat.m_Ambient));
        glUniform3fv(diffuseLoc, 1, glm::value_ptr(mat.m_Diffuse));
        glUniform3fv(specularLoc, 1, glm::value_ptr(mat.m_Specular));
        glUniform1f(shininessLoc, mat.Shininess);
        // send light to shaders
        glUniform3fv(lightPositionLoc, 1, glm::value_ptr(envL.m_Position));
        glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(envL.m_Ambient));
        glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(mat.m_Specular));
        glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(mat.m_Diffuse));
        // seeing as we only have a single VAO there's no need to bind it every time,
        // but we'll do so to keep things a bit more organized
        glBindVertexArray(instCube->getModel()->VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0); // no need to unbind it every time
        // glUseProgram(shaderProgram);

        float time = glfwGetTime();
        instCube->setTranslation(glm::vec3(0.f, 0.f, 0.f));
        instCube->setRotation(glm::vec3(0.f, 1.f, 1.f), time * 3);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(instCube->transform));
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        glUseProgram(shaderProgram);




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &instCube->getModel()->VAO);
    glDeleteBuffers(1, &instCube->getModel()->VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
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
