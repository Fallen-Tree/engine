#pragma once

#include <glad/glad.h>
#include "math_types.hpp"
#include "geometry_primitives.hpp"
#include "input.hpp"
#include "engine_config.hpp"
#include "user_config.hpp"

class Camera {
 public:
    // constructor with vectors
    explicit Camera(Vec3 translation, Vec3 up = Vec3(0.0f, 1.0f, 0.0f),
            Vec3 scale = Vec3(1), Mat4 rotation = Mat4(1),
            float nearPlane = DFL_NEAR_PLANE, float farPlane = DFL_FAR_PLANE);

    void SetTransform(Transform transform, Vec3 up = Vec3(0.0f, 1.0f, 0.0f));
    void SetPosition(Vec3 position);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    Mat4 GetViewMatrix();
    Mat4 GetProjectionMatrix();

    Ray GetRayThroughScreenPoint(Vec2 pos);

    float GetZoom();
    void SetZoom(float zoom);

    Vec3 GetPosition();
    Vec3 GetFront();

    void SetScreenSize(Vec2);
    Vec2 GetScreenSize();

    void Update(Input * input, float deltaTime);

 private:
    // Transform class
    Transform m_Transform;

    // camera Attributes
    Vec3 m_Front;
    Vec3 m_Up;
    Vec3 m_Right;
    Vec3 m_WorldUp;

    // camera options
    float m_MouseSensitivity;
    float m_Zoom;

    float m_NearPlane, m_FarPlane;
    Vec2 m_ScreenSize;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
};

