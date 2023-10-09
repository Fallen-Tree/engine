#ifndef SRC_CAMERA_CAMERA_H_
#define SRC_CAMERA_CAMERA_H_

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

const float MAXFOV = 45.0f;
const float MINFOV = 1.0f;


class Camera {
 public:
    // constructor with vectors
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -3.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = YAW, float pitch = PITCH);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    float GetZoom();

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
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};

#endif  // SRC_CAMERA_CAMERA_H_
