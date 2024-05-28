#include "camera.hpp"

#include "input.hpp"
#include "math_types.hpp"
#include "math.h"
#include "user_config.hpp"
#include "logger.hpp"
#include "sound.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


Vec3 NewFront(Mat4 rotation);

Camera::Camera(Vec3 translation, Vec3 up,
        Vec3 scale, Mat4 rotation,
        float nPlane, float fPlane) {
    m_Transform  = Transform(translation, scale, rotation);
    this->m_WorldUp = up;
    this->m_Front = STARTING_POSITION;
    this->m_MouseSensitivity = SENSIVITY;
    this->m_Zoom = DFL_ZOOM;
    m_NearPlane = nPlane;
    m_FarPlane = fPlane;
    UpdateCameraVectors();
}

void Camera::SetTransform(Transform transform, Vec3 up) {
    m_Transform = transform;
    this->m_WorldUp = up;
    this->m_Front = STARTING_POSITION;
    this->m_Zoom = DFL_ZOOM;
    UpdateCameraVectors();
}

void Camera::SetPosition(Vec3 pos) {
    m_Transform.SetTranslation(pos);
    UpdateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt
// Matrix
Mat4 Camera::GetViewMatrix() {
    return glm::lookAt(m_Transform.GetTranslation(),
            m_Transform.GetTranslation() + this->m_Front, this->m_Up);
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

void Camera::SetZoom(float zoom) {
    this->m_Zoom = zoom;
}

Vec3 Camera::GetPosition() {
    return m_Transform.GetTranslation();
}

Vec3 Camera::GetFront() {
    return this->m_Front;
}

void Camera::SetScreenSize(Vec2 size) {
    m_ScreenSize = size;
}

Vec2 Camera::GetScreenSize() {
    return m_ScreenSize;
}

float Camera::GetNearPlane() {
    return m_NearPlane;
}

float Camera::GetFarPlane() {
    return m_FarPlane;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::UpdateCameraVectors() {
    this->m_Front = glm::normalize(
        Mul(STARTING_POSITION, glm::transpose(m_Transform.GetRotation()))
    );
    // also re-calculate the Right and Up vector
    this->m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    this->m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}

void Camera::Update(Input * input, float deltaTime) {
    UpdateCameraVectors();
    // Set Sounds Listener position
    Vec3 pos = m_Transform.GetTranslation();
    BASS_3DVECTOR bpos = {pos.x, pos.y, pos.z};
    BASS_3DVECTOR bup = {m_WorldUp.x, m_WorldUp.y, m_WorldUp.z};
    // don't know why "-" before lookDirection
    BASS_3DVECTOR bfront = {-m_Front.x, -m_Front.y, -m_Front.z};

    BASS_Set3DPosition(&bpos, NULL, &bfront, &bup);
    BASS_Apply3D();
}
