#include "input.hpp"
#include <vector>

// should send to all constants
const int maxValidKey = 350;

Input::Input(GLFWwindow *window) {
    m_IsDown = std::vector<bool>(maxValidKey, false);
    m_Window = window;
    SetMode();
}

Input::Input() {
    m_IsDown = std::vector<bool>(maxValidKey, false);
}

bool Input::IsKeyPressed(Key button) {
    return m_IsDown[button];
}

bool Input::IsKeyDown(Key button) {
    return glfwGetKey(m_Window, button) == GLFW_PRESS;
}

float Input::ScrollOfsset() {
    return this->m_ScrollOffset;
}

float Input::MouseX() {
    return this->m_Current.x;
}

float Input::MouseY() {
    return this->m_Current.y;
}

float Input::OffsetX() {
    return this->m_Current.x - this->m_Last.x;
}

float Input::OffsetY() {
    return this->m_Last.y - this->m_Current.y;
}

void Input::SetWindow(GLFWwindow * window) {
    this->m_Window = window;
}

void Input::SetMouseX(float x) {
    m_Last.x = m_Current.x;
    m_Current.x = x;
}

void Input::SetMouseY(float y) {
    m_Last.y = m_Current.y;
    m_Current.y = y;
}

void Input::SetScrollOffset(float offset) {
    this->m_ScrollOffset = offset;
}

void Input::Update() {
    double xpos, ypos;
    glfwGetCursorPos(m_Window, &xpos, &ypos);
    SetMouseX(static_cast<float>(xpos));
    SetMouseY(static_cast<float>(ypos));
    std::fill(m_IsDown.begin(), m_IsDown.end(), false);
    SetScrollOffset(0);
}

void Input::SetMode() {
    glfwSetInputMode(m_Window, MODE, VALUE);
}

void Input::ButtonPress(int button) {
    m_IsDown[button] = true;
}
