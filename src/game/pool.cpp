#pragma once

#include <limits>
#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "logger.hpp"

#include "classes.cpp"

float gravity = 9.8;
Model* ball_model = nullptr;
// Classes for pool
class MovingBall : public Behaviour {
 public:
    static Object New(Vec3 position, float radius,
     std::string diffuseSource, float mass = 1.0) {
        Transform *transform = new Transform(position, Vec3(radius), Mat4(1.0));

        if (ball_model == nullptr) ball_model = Model::loadFromFile("shar_1200.obj");
        Model *model = ball_model;
        Material sphereMaterial = {4.f, Texture(diffuseSource)};
        model->setMaterial(sphereMaterial);

        Collider *collider = new Collider{Sphere{Vec3(0.0), 1.0}};
        RigidBody *rb = new RigidBody(mass, IBodySphere(1, mass),
                Vec3(0), 0.6f, Vec3(0, -mass * gravity, 0), Vec3(1), Vec3(1), 0.1f);
        return newDynamicBody<MovingBall>(transform, model, collider, rb);
    }

    void Update(float dt) override {
        // Vec3 pos = self.GetTransform()->GetTranslation();
        // Logger::Info("ball pos: %f %f %f", pos.x, pos.y, pos.z);
    }
};

class Hole : public TriggerArea {
 public:
    void OnCollision(Collider other) {
        return;
        // if getCollisions returns colliders then I need a function to get objects from it.
        // Object obj = other.GetObject();

        // check if some object is a ball and not a table or a player
        // can be done with collision layers or with some info about objects.
        // if (obj is ball) {
        //     Consume(obj);
        // }
    }

    void Consume(MovingBall *ball) {
        // delete ball
        // for now just move with animation
        ball->self.GetRigidBody()->velocity = Vec3(0, 0, 0);
        ball->self.GetTransform()->SetTranslation(self.GetTransform()->GetTranslation());
        ball->self.AddAnimation();
        auto newTrans = *ball->self.GetTransform();
        newTrans.Translate(Vec3(0, -2, 0));
        ball->self.GetAnimation()->addAnimation(newTrans, 2.f);
        // add score
        // how???
    }

    // i hate that i need to override Update even if it is empty
    void Update(float dt) override {}
};

class Cue : public Behaviour {
 public:
    static Object New(std::vector<MovingBall *> objects, Camera *camera) {
        Model *model = Model::loadFromFile("/cue.obj");
        Material material = {
            4.f,
            Texture("/kiy.png"),
        };
        model->setMaterial(material);
        Transform *transform = new Transform(Vec3(0), Vec3(8), Mat4(0));
        Object newCue = newModel<Cue>(transform, model);
        newCue.AddAnimation();
        reinterpret_cast<Cue*>(newCue.GetBehaviour())->Init(objects, camera);
        return newCue;
    }

    void Init(std::vector<MovingBall *> objects, Camera *camera) {
        m_Objects = objects;
        m_Camera = camera;
        m_CueDistance = 1.f;
        m_AttackVelocity = 12.f;
        m_Attacking = false;
    }

    void Update(float dt) override {
        Ray ray = Ray(m_Camera->GetPosition(), m_Camera->GetPosition() + m_Camera->GetFront());
        MovingBall *target = nullptr;
        const float MAX_HIT_DIST = 1e9;
        float closest = MAX_HIT_DIST;
        for (int i = 0; i < m_Objects.size(); i++) {
            auto obj = m_Objects[i];
            auto hit = obj->self.GetCollider()->RaycastHit(*obj->self.GetTransform(), ray);
            if (hit && *hit < closest) {
                target = obj;
                closest = *hit;
            }
        }

        if (s_Input->IsKeyPressed(Key::MouseLeft)) {
            if (m_CurrentTarget != nullptr) {
                m_Attacking = true;
                Vec3 myPos = self.GetTransform()->GetTranslation();
                Vec3 targetPos = m_CurrentTarget->self.GetTransform()->GetTranslation();
                Vec3 direction = glm::normalize(targetPos - myPos);
                auto nextTransform = *self.GetTransform();
                nextTransform.Translate(0.8f * direction);
                self.GetAnimation()->addAnimation(nextTransform, .15f);
            }

            if (m_CurrentTarget == nullptr && target != nullptr)
                m_CurrentTarget = target;
        }

        if (self.GetAnimation()->isComplete()) {
            if (m_Attacking) {
                Vec3 nextPos = self.GetTransform()->GetTranslation();
                Vec3 targetPos = m_CurrentTarget->self.GetTransform()->GetTranslation();
                Vec3 direction = glm::normalize(targetPos - nextPos);
                direction.y = 0.f;
                m_CurrentTarget->self.GetRigidBody()->velocity += m_AttackVelocity * direction;
                m_CurrentTarget = nullptr;
            }
            m_Attacking = false;
        }

        if (m_Attacking)
            return;

        if (s_Input->IsKeyPressed(Key::MouseRight))
            m_CurrentTarget = nullptr;

        if (m_CurrentTarget != nullptr) {
            Vec3 center = m_CurrentTarget->self.GetTransform()->GetTranslation();
            Vec3 closest = ray.origin + glm::dot(center - ray.origin, ray.direction) * ray.direction;
            closest.y = center.y;
            Vec3 onCircle = center + glm::normalize(closest - center) * m_CueDistance;
            self.GetTransform()->SetTranslation(onCircle);

            Vec3 toCenter = center - onCircle;
            float angle = glm::acos(glm::dot(toCenter, ray.direction) / m_CueDistance);
            self.GetTransform()->SetRotation(-glm::pi<float>()/2 + 0.06, glm::cross(Vec3{0.f, 1.f, 0.f}, toCenter));
        } else {
            self.GetTransform()->SetRotation(glm::pi<float>(), Vec3(1.f, 0.f, 0.f));
            self.GetTransform()->SetTranslation(m_Camera->GetPosition() + Vec3{0.5f, -0.5f, -0.5f});
        }
     }

 private:
    bool m_Attacking;
    float m_CueDistance;
    float m_AttackVelocity;
    MovingBall *m_CurrentTarget = nullptr;
    std::vector<MovingBall *> m_Objects;
    Camera *m_Camera;
};

class GameManager : public Behaviour {
 private:
    int score = 0;
    PublicText text;
    void ShowScore() {
        text.SetContent("Score: " + std::to_string(score));
    }

 public:
    void Update(float dt) override {
        ShowScore();
    }
};

class Table : public Behaviour {
 public:
    static Object New(Vec3 position, Vec3 scale) {
        Transform *transform = new Transform(position, scale, Mat4(1.0));

        Model *model = Model::loadFromFile("/stol_1.obj");
        Material material = Material{
            4.f,
            Texture("/stol.png"),
        };
        model->setMaterial(material);

        // get full mesh of the table or make multiple objects for walls of the table.
        // Collider *col = new Collider {&model->meshes[0]};
        float h = 0.9;

        float width = 0.7;
        float length = 0.4;

        Collider *col = new Collider{AABB {
            Vec3{-width, -0.5, -length},
            Vec3{width, h, length},
        }};
        Object obj = newStaticBody<Table>(transform, model, col);

        float wall_height = 0.5;
        AABB walls[] = {
            AABB {Vec3(width + 0.01, h + 0.01, -length), Vec3(width + 1, h + wall_height, length)},
            AABB {Vec3(-width - 1+ 0.01, h + 0.01, -length), Vec3(-width, h + wall_height, length)},
            AABB {Vec3(-width+ 0.01, h + 0.01, length), Vec3(width, h + wall_height, length + 1)},
            AABB {Vec3(-width+ 0.01, h + 0.01, -length - 1), Vec3(width, h + wall_height, -length)}
        };
        for (int i = 0; i < 4; ++i) {
            Collider *col = new Collider{walls[i]};
            newStaticBody(transform, col);
        }

        return obj;
    }

    void Update(float dt) override {}
};