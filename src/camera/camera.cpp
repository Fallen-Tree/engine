#include "camera.hpp"

Camera::Camera(glm::vec3 position,
                glm::vec3 up, float yaw,
                float pitch) {
    this->m_Position = position;
    this->m_WorldUp = up;
    this->m_Yaw = yaw;
    this->m_Pitch = pitch;
    this->m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    this->m_MovementSpeed = SPEED;
    this->m_MouseSensitivity = SENSITIVITY;
    this->m_Zoom = ZOOM;
    UpdateCameraVectors();
}

void Camera::SetPosition(glm::vec3 position,
                glm::vec3 up, float yaw,
                float pitch) {
    this->m_Position = position;
    this->m_WorldUp = up;
    this->m_Yaw = yaw;
    this->m_Pitch = pitch;
    this->m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    this->m_Zoom = ZOOM;
    UpdateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt
// Matrix
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(this->m_Position, this->m_Position + this->m_Front, this->m_Up);
}

float Camera::GetZoom() {
    return this->m_Zoom;
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
        if (this->m_Pitch > MAXPITCH)  this->m_Pitch = MAXPITCH;
        if (this->m_Pitch < MINPITCH) this->m_Pitch = MINPITCH;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    this->m_Zoom -= static_cast<float>(yoffset);
    if (this->m_Zoom < MINFOV) this->m_Zoom = MINFOV;
    if (this->m_Zoom > MAXFOV) this->m_Zoom = MAXFOV;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::UpdateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
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
