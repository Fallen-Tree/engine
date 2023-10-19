#include "controller.hpp"

Input::Input(GLFWwindow *window) {
    m_Window = window;
    glfwSetInputMode(m_Window, MODE, VALUE);
}

bool Input::IsKeyPressed(key button) {
    return glfwGetKey(m_Window, button) == GLFW_PRESS;
}

float Input::ScrollOfsset() {
    return this->m_ScrollOffset;
}

float Input::MouseX() {
    return this->m_X.y;
}

float Input::MouseY() {
    return this->m_Y.y;
}

float Input::OffsetX() {
    return this->m_X.y - this->m_X.x;
}

float Input::OffsetY() {
    return this->m_Y.x - this->m_Y.y;
}

void Input::setWindow(GLFWwindow * window) {
    this->m_Window = window;
}

void Input::setMouseX(float x) {
    m_X.x = m_X.y;
    m_X.y = x;
}

void Input::setMouseY(float y) {
    m_Y.x = m_Y.y;
    m_Y.y = y;
}

void Input::setScrollOffset(float offset) {
    this->m_ScrollOffset = offset;
}

void Input::update() {
    double xpos, ypos;
    glfwGetCursorPos(m_Window, &xpos, &ypos);
    setMouseX(static_cast<float>(xpos));
    setMouseY(static_cast<float>(ypos));
}
