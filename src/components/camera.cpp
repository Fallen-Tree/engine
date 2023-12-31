#include "camera.hpp"
#include "input.hpp"
#include "math_types.hpp"
#include "user_config.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(Vec3 position,
                Vec3 up, float yaw,
                float pitch) {
    this->m_Position = position;
    this->m_WorldUp = up;
    this->m_Yaw = yaw;
    this->m_Pitch = pitch;
    this->m_Front = Vec3(0.0f, 0.0f, -1.0f);
    this->m_MovementSpeed = DFL_SPEED;
    this->m_MouseSensitivity = SENSIVITY;
    this->m_Zoom = DFL_ZOOM;
    UpdateCameraVectors();
}

void Camera::SetPosition(Vec3 position,
                Vec3 up, float yaw,
                float pitch) {
    this->m_Position = position;
    this->m_WorldUp = up;
    this->m_Yaw = yaw;
    this->m_Pitch = pitch;
    this->m_Front = Vec3(0.0f, 0.0f, -1.0f);
    this->m_Zoom = DFL_ZOOM;
    UpdateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt
// Matrix
Mat4 Camera::GetViewMatrix() {
    return glm::lookAt(this->m_Position, this->m_Position + this->m_Front, this->m_Up);
}

float Camera::GetZoom() {
    return this->m_Zoom;
}

Vec3 Camera::GetPosition() {
    return this->m_Position;
}

Vec3 Camera::GetFront() {
    return this->m_Front;
}

// processes input received from any keyboard-like input system. Accepts
// input parameter in the form of camera defined ENUM (to abstract it from
// windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = m_MovementSpeed * deltaTime;
    if (direction == FORWARD)  this->m_Position += this->m_Front * velocity;
    if (direction == BACKWARD) this->m_Position -= this->m_Front * velocity;
    if (direction == LEFT)     this->m_Position -= this->m_Right * velocity;
    if (direction == RIGHT)    this->m_Position += this->m_Right * velocity;
}

// processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    this->m_Yaw += xoffset;
    this->m_Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get
    // flipped
    if (constrainPitch) {
        if (this->m_Pitch > MAX_PITCH)  this->m_Pitch = MAX_PITCH;
        if (this->m_Pitch < MIN_PITCH) this->m_Pitch = MIN_PITCH;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    this->m_Zoom -= static_cast<float>(yoffset);
    if (this->m_Zoom < MIN_FOV) this->m_Zoom = MIN_FOV;
    if (this->m_Zoom > MAX_FOV) this->m_Zoom = MAX_FOV;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::UpdateCameraVectors() {
    // calculate the new Front vector
    Vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));


    // normalize the vectors, because their length gets closer to 0 the more you look
    // up or down which results in slower movement.
    this->m_Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    this->m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    this->m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}

void Camera::Update(Input * input, float deltaTime) {
    float xOffset = input->OffsetX();
    float yOffset = input->OffsetY();
    if (xOffset != 0 || yOffset != 0) {
        ProcessMouseMovement(xOffset, yOffset);
    }

    float scrollOffset = input->ScrollOfsset();
    if (scrollOffset != 0)
        ProcessMouseScroll(scrollOffset);

    if (input->IsKeyDown(Key::W))
        ProcessKeyboard(FORWARD, deltaTime);
    if (input->IsKeyDown(Key::S))
        ProcessKeyboard(BACKWARD, deltaTime);
    if (input->IsKeyDown(Key::A))
        ProcessKeyboard(LEFT, deltaTime);
    if (input->IsKeyDown(Key::D))
        ProcessKeyboard(RIGHT, deltaTime);
}
