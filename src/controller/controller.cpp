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
    return this->m_CurrentX;
}

float Input::MouseY() {
    return this->m_CurrentY;
}

float Input::OffsetX() {
    return this->m_CurrentX - this->m_LastX;
}

float Input::OffsetY() {
    return this->m_LastY - this->m_CurrentY;
}

void Input::setWindow(GLFWwindow * window) {
    this->m_Window = window;
}

void Input::setMouseX(float x) {
    this->m_LastX = this->m_CurrentX;
    this->m_CurrentX = x;
}

void Input::setMouseY(float y) {
    this->m_LastY = this->m_CurrentY;
    this->m_CurrentY = y;
}

void Input::setScrollOffset(float offset) {
    this->m_ScrollOffset = offset;
}
