#include "input.hpp"

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

void Input::SetWindow(GLFWwindow * window) {
    this->m_Window = window;
}

void Input::SetMouseX(float x) {
    m_X.x = m_X.y;
    m_X.y = x;
}

void Input::SetMouseY(float y) {
    m_Y.x = m_Y.y;
    m_Y.y = y;
}

void Input::SetScrollOffset(float offset) {
    this->m_ScrollOffset = offset;
}

void Input::Update() {
    double xpos, ypos;
    glfwGetCursorPos(m_Window, &xpos, &ypos);
    SetMouseX(static_cast<float>(xpos));
    SetMouseY(static_cast<float>(ypos));
}
