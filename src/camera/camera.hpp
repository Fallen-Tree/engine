#ifndef SRC_CAMERA_CAMERA_HPP_
#define SRC_CAMERA_CAMERA_HPP_

#include <glad/include/glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Initial (Default) camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

const float MAXPITCH = 89.0f;
const float MINPITCH = -89.0f;

const float MAXFOV = 45.0f;
const float MINFOV = 1.0f;




class Camera {
 public:
    // constructor with vectors
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -3.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = YAW, float pitch = PITCH);

    void SetPosition(glm::vec3 position = glm::vec3(0.0f, 0.0f, -3.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = YAW, float pitch = PITCH);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    float GetZoom();

    glm::vec3 GetPosition();

    // processes input received from any keyboard-like input system.
    // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);


    // processes input received from a mouse input system.
    // Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);


    // processes input received from a mouse scroll-wheel event.
    // Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);


 private:
    // camera Attributes
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    // euler Angles
    float m_Yaw;
    float m_Pitch;

    // camera options
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
};

#endif  // SRC_CAMERA_CAMERA_HPP_
