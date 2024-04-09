#include <math.h>

#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "logger.hpp"

#include "classes.cpp"

extern Engine *engine;

class PlayerController : public Behaviour {
 private:
    enum MovementMode {Walk, Run, Fly};
    Camera * m_Camera;
    float m_FlySpeed = 3;
    float m_RunSpeed = 10;
    float m_WalkSpeed = 5;
    float m_MouseSensitivity = SENSIVITY;
    Vec3 lastStandPosition;
    MovementMode m_MovementMode = Walk;
    int camShakeDir = 1;
    Vec3 cameraOffset = Vec3(0);

    void ProcessKeyboard(float deltaTime) {
        Transform *tr = self.GetTransform();

        if (s_Input->IsKeyPressed(Key::F)) {
            if (m_MovementMode == Fly) {
                m_MovementMode = Walk;
                tr->SetTranslation(lastStandPosition);
            } else {
                m_MovementMode = Fly;
                lastStandPosition = tr->GetTranslation();
            }
        }
        if (m_MovementMode != Fly) {
            if (s_Input->IsKeyDown(Key::LShift))
                m_MovementMode = Run;
            else
                m_MovementMode = Walk;
        }

        Vec3 front = m_Camera->GetFront();
        if (m_MovementMode != Fly) {
            front.y = 0;
        }
        front = glm::normalize(front);
        Vec3 right = glm::normalize(glm::cross(front, Vec3(0, 1, 0)));

        Vec3 direction = Vec3(0);

        if (s_Input->IsKeyDown(Key::W))
            direction += front;
        if (s_Input->IsKeyDown(Key::S))
            direction -= front;
        if (s_Input->IsKeyDown(Key::A))
            direction -= right;
        if (s_Input->IsKeyDown(Key::D))
            direction += right;
        if (direction != Vec3(0))
            direction = glm::normalize(direction);
        float moveSpeed;
        if (m_MovementMode == Walk)
            moveSpeed = m_WalkSpeed;
        else if (m_MovementMode == Run)
            moveSpeed = m_RunSpeed;
        else /*m_MovementMode == Fly*/
            moveSpeed = m_FlySpeed;

        Vec3 velocity = direction * moveSpeed;
        if (m_MovementMode != Fly) {
            cameraOffset += Vec3(0, -0.1, 0) * (glm::length(velocity) * camShakeDir) * deltaTime;
            if (glm::length(cameraOffset) > 0.08) camShakeDir = - camShakeDir;
            cameraOffset *= pow(0.05, deltaTime);
        } else {
            cameraOffset = Vec3(0);
        }
        tr->Translate(velocity * deltaTime);
    }


    void ProcessMouseMovement() {
        float xOffset = s_Input->OffsetX();
        float yOffset = s_Input->OffsetY();

        xOffset *= m_MouseSensitivity;
        yOffset *= m_MouseSensitivity;
 
        Transform *tr = self.GetTransform();

        tr->Rotate(0, xOffset, 0);
        tr->RotateGlobal(-yOffset, 0, 0);

        // to update front vector
        m_Camera->SetTransform(*self.GetTransform());
 
        Vec3 front = m_Camera->GetFront();
        if (abs(glm::dot(front, Vec3(0, 1, 0))) > MAX_DOT) {
            tr->RotateGlobal(yOffset, 0, 0);
        }
    }

    void ProcessMovement(float deltaTime) {
        if (m_Camera == nullptr) return;

        ProcessMouseMovement();

        ProcessKeyboard(deltaTime);

        Transform newCamTransform = *self.GetTransform();
        newCamTransform.Translate(cameraOffset);
        m_Camera->SetTransform(newCamTransform);
    }

 public:
    void Update(float deltaTime) {
        ProcessMovement(deltaTime);
    }

    explicit PlayerController(Camera * camera) {
        m_Camera = camera;
    }
};
