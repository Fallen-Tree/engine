#include "camera.h"

Camera::Camera(glm::vec3 position,
                glm::vec3 up, float yaw,
                float pitch) {
    this->Position = position;
    this->WorldUp = up;
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->Front = glm::vec3(0.0f, 0.0f, -1.0f);
    this->MovementSpeed = SPEED;
    this->MouseSensitivity = SENSITIVITY;
    this->Zoom = ZOOM;
    updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt
// Matrix
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

float Camera::GetZoom() {
    return this->Zoom;
}

// processes input received from any keyboard-like input system. Accepts
// input parameter in the form of camera defined ENUM (to abstract it from
// windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD) this->Position += this->Front * velocity;
    if (direction == BACKWARD) this->Position -= this->Front * velocity;
    if (direction == LEFT) this->Position -= this->Right * velocity;
    if (direction == RIGHT) this->Position += this->Right * velocity;
}

// processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    this->Yaw += xoffset;
    this->Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get
    // flipped
    if (constrainPitch) {
        if (this->Pitch > 89.0f) this->Pitch = 89.0f;
        if (this->Pitch < -89.0f) this->Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    this->Zoom -= static_cast<float>(yoffset);
    if (this->Zoom < MINFOV) this->Zoom = MINFOV;
    if (this->Zoom > MAXFOV) this->Zoom = MAXFOV;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));


    // normalize the vectors, because their length gets closer to 0 the more you look
    // up or down which results in slower movement.
    this->Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    this->Right = glm::normalize(glm::cross(Front, WorldUp));
    this->Up    = glm::normalize(glm::cross(Right, Front));
}
