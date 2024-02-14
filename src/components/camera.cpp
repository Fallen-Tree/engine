#include "camera.hpp"
#include "input.hpp"
#include "math_types.hpp"
#include "math.h"
#include "user_config.hpp"
#include "logger.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

Vec3 Mul(Vec3 vec, Mat4 mat) {
    Vec4 res = Vec4(vec, 1.0) * mat;
    return Vec3(res.x / res.w, res.y / res.w, res.z / res.w);
}

Camera::Camera(Vec3 translation, Vec3 up,
        Vec3 scale, Mat4 rotation,
        float nPlane, float fPlane) {
    tranform  = Transform(translation, scale, rotation);
    this->m_WorldUp = up;
    this->m_Front = Vec3(0.0f, 0.0f, -1.0f);
    this->m_MovementSpeed = DFL_SPEED;
    this->m_MouseSensitivity = SENSIVITY;
    this->m_Zoom = DFL_ZOOM;
    m_NearPlane = nPlane;
    m_FarPlane = fPlane;
    UpdateCameraVectors();
}

void Camera::SetPosition(Vec3 translation, Vec3 up,
        Vec3 scale, Mat4 rotation) {
    tranform  = Transform(translation, scale, rotation);
    this->m_WorldUp = up;
    this->m_Front = Vec3(0.0f, 0.0f, -1.0f);
    this->m_Zoom = DFL_ZOOM;
    UpdateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt
// Matrix
Mat4 Camera::GetViewMatrix() {
    return glm::lookAt(tranform.GetTranslation(),
            tranform.GetTranslation() + this->m_Front, this->m_Up);
}

Mat4 Camera::GetProjectionMatrix() {
    float aspectRatio = m_ScreenSize.x / m_ScreenSize.y;
    return glm::perspective(glm::radians(m_Zoom), aspectRatio, m_NearPlane, m_FarPlane);
}

Ray Camera::GetRayThroughScreenPoint(Vec2 pos) {
    pos.y = m_ScreenSize.y - pos.y;
    pos -= m_ScreenSize / 2.0f;
    float halfTan = glm::tan(glm::radians(m_Zoom)/2);
    float aspectRatio = m_ScreenSize.x / m_ScreenSize.y;
    Vec3 unprojectedNear = Vec3 {
        pos.x * 2 * m_NearPlane * halfTan * aspectRatio / m_ScreenSize.x,
        pos.y * 2 * m_NearPlane * halfTan / m_ScreenSize.y,
        -m_NearPlane,
    };
    Vec3 unprojectedFar = Vec3 {
        pos.x * 2 * m_FarPlane * halfTan * aspectRatio / m_ScreenSize.x,
        pos.y * 2 * m_FarPlane * halfTan / m_ScreenSize.y,
        -m_FarPlane,
    };

    auto mat = (glm::transpose(glm::inverse(GetViewMatrix())));
    auto mul = [](Vec3 v, Mat4 mat) {
        Vec4 res = Vec4(v, 1) * mat;
        return Vec3(res.x / res.w, res.y / res.w, res.z / res.w);
    };
    return Ray(mul(unprojectedNear, mat), mul(unprojectedFar, mat));
}

float Camera::GetZoom() {
    return this->m_Zoom; 
}

Vec3 Camera::GetPosition() {
    return tranform.GetTranslation();
}

Vec3 Camera::GetFront() {
    return this->m_Front;
}

void Camera::SetScreenSize(Vec2 size) {
    m_ScreenSize = size;
}

// processes input received from any keyboard-like input system. Accepts
// input parameter in the form of camera defined ENUM (to abstract it from
// windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = m_MovementSpeed * deltaTime;
    Vec3 newTranslation = tranform.GetTranslation();
    if (direction == FORWARD)  newTranslation += this->m_Front * velocity;
    if (direction == BACKWARD) newTranslation -= this->m_Front * velocity;
    if (direction == LEFT)     newTranslation -= this->m_Right * velocity;
    if (direction == RIGHT)    newTranslation += this->m_Right * velocity;
    tranform.SetTranslation(newTranslation);
}

// processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {

    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    auto rotate = tranform.GetRotation();
    auto t = glm::rotate(rotate, yoffset, Mul(Vec3(1.f, 0.f, 0.f), rotate)); 
    Vec3 nextFront = NewFront(glm::rotate(rotate, yoffset, Mul(Vec3(1.f, 0.f, 0.f), rotate))); 
    if (constrainPitch && abs(glm::dot(nextFront, m_WorldUp)) > 0.95) return; 
    tranform.SetRotation(t);

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

Vec3 Camera::NewFront(Mat4 rotation) {
    Vec3 front = Mul(Vec3(0, 0, -1), rotation);
    // normalize the vectors, because their length gets closer to 0 the more you look
    // up or down which results in slower movement.
    return glm::normalize(Mul(Vec3(0, 0, -1), rotation));
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::UpdateCameraVectors() {
    this->m_Front = NewFront(tranform.GetRotation());
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
