#ifndef SRC_CAMERA_CAMERA_HPP_
#define SRC_CAMERA_CAMERA_HPP_

#include <glad/glad.h>
#include "math_types.hpp"
#include "geometry_primitives.hpp"
#include "input.hpp"
#include "engine_config.hpp"
#include "user_config.hpp"

// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
 public:
    // constructor with vectors
    explicit Camera(Vec3 position, Vec3 up = Vec3(0.0f, 1.0f, 0.0f),
                    float yaw = DFL_YAW, float pitch = DFL_PITCH,
                    float nearPlane = DFL_NEAR_PLANE, float farPlane = DFL_FAR_PLANE);

    void SetPosition(Vec3 position = Vec3(0.0f, 0.0f, -3.0f),
                    Vec3 up = Vec3(0.0f, 1.0f, 0.0f),
                    float yaw = DFL_YAW, float pitch = DFL_PITCH);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    Mat4 GetViewMatrix();
    Mat4 GetProjectionMatrix();

    Ray GetRayThroughScreenPoint(Vec2 pos);

    float GetZoom();

    Vec3 GetPosition();
    Vec3 GetFront();

    void SetScreenSize(Vec2);

    // processes input received from any keyboard-like input system.
    // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);


    // processes input received from a mouse input system.
    // Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);


    // processes input received from a mouse scroll-wheel event.
    // Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    void Update(Input * input, float deltaTime);

 private:
    // camera Attributes
    Vec3 m_Position;
    Vec3 m_Front;
    Vec3 m_Up;
    Vec3 m_Right;
    Vec3 m_WorldUp;

    // euler Angles
    float m_Yaw;
    float m_Pitch;

    // camera options
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;

    float m_NearPlane, m_FarPlane;
    Vec2 m_ScreenSize;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
};

#endif  // SRC_CAMERA_CAMERA_HPP_
