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
    float m_FlySpeed = 25;
    float m_RunSpeed = 15;
    float m_WalkSpeed = 10;
    float m_MouseSensitivity = SENSIVITY;
    Vec3 lastStandPosition;
    MovementMode m_MovementMode = Walk;
    int camShakeDir = 1;
    float jump_power = 8;
    float mass = 5.f;
    Vec3 cameraOffset = Vec3(0);

    void ProcessKeyboard(float deltaTime) {
        Transform *tr = self.GetTransform();
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
        if (m_MovementMode != Fly) {
            velocity.y = self.GetRigidBody()->velocity.y;
            if (s_Input->IsKeyPressed(Key::Space)) {  // add check if grounded
                velocity += Vec3(0, jump_power, 0);
            }
        }
        self.GetRigidBody()->velocity = velocity;
    }


    void ProcessMouseMovement() {
        float xOffset = s_Input->OffsetX();
        float yOffset = s_Input->OffsetY();

        xOffset *= m_MouseSensitivity;
        yOffset *= m_MouseSensitivity;

        Transform *tr = self.GetTransform();

        tr->RotateGlobal(0, -xOffset, 0);
        tr->Rotate(yOffset, 0, 0);

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

    std::vector<Object> m_InteractableObjects;
    PublicText * m_HintText = nullptr;
    Object holdObj;
    Vec3 holdObjDefaultForce;
    Object eatSound;
    bool hasHoldObj = false;
    const float MAX_HIT_DIST = 15.0f;
    const float throwPower = 20.0f;
    const std::string FOOD_NAME = "Pizza";

    void TakeObject(Object o) {
        holdObj = o;
        self.AddChild(holdObj, true);
        hasHoldObj = true;
        holdObjDefaultForce = holdObj.GetRigidBody()->defaultForce; 
        holdObj.GetRigidBody()->defaultForce = Vec3(0);
    }

    void ReleaseObject() {
        hasHoldObj = false;
        self.RemoveChild(holdObj, true);
        holdObj.GetRigidBody()->defaultForce = holdObjDefaultForce;
    }

    void ProcessTargeting(float deltaTime) {
        if (m_MovementMode == Fly) {
            ReleaseObject();
            return;
        }
        Ray ray = Ray(m_Camera->GetPosition(), m_Camera->GetPosition() + m_Camera->GetFront());
        auto target = self.GlobalRaycast(ray, Collider::Layer4, MAX_HIT_DIST);

        if (hasHoldObj) {
            // how to move to new line in this text?
            m_HintText->SetContent("E to drop / Q to throw");
        } else if (target) {
            if (target->GetName() == FOOD_NAME) {
                m_HintText->SetContent("press E to eat");
            } else {
                m_HintText->SetContent("press E to grab");
            }
        } else {
            m_HintText->SetContent("");
        }

        if (s_Input->IsKeyPressed(Key::E)) {
            if (hasHoldObj) {
                ReleaseObject();
            } else if (target) {
                if (target->GetName() == FOOD_NAME) {
                    target->Remove();
                    eatSound.GetSound()->Start();
                } else {
                    TakeObject(*target);
                }
            }
        }
        if (s_Input->IsKeyPressed(Key::Q) && hasHoldObj) {
            ReleaseObject();
            Vec3 throwVelocity = glm::normalize(m_Camera->GetFront()) * throwPower;
            holdObj.GetRigidBody()->velocity = throwVelocity;
        }

        if (hasHoldObj) {
            holdObj.GetRigidBody()->velocity = Vec3(0);
            auto global = holdObj.GetGlobalTransform();
            holdObj.GetTransform()->Rotate(glm::inverse(global.GetRotation()));
        }
    }

 public:
    void Update(float deltaTime) {
        ProcessMovement(deltaTime);
        Vec3 pos = self.GetTransform()->GetTranslation();
        ProcessTargeting(deltaTime);
    }

    explicit PlayerController(Camera * camera, PublicText * hintText) {
        m_Camera = camera;
        m_HintText = hintText;
        eatSound = engine->NewObject();
        eatSound.AddSound(SoundType::SOUND_FLAT, "chew.wav").SetVolume(0.5f);
    }
};
