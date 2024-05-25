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
     std::string diffuseSource, float mass = 3.0) {
        Transform *transform = new Transform(position, Vec3(radius), Mat4(1.0));

        if (ball_model == nullptr) ball_model = engine->GetModelManager().LoadModel("pool/shar_1200.obj");
        Model *model = ball_model;
        Material sphereMaterial = {4.f, Texture(diffuseSource)};
        model->setMaterial(sphereMaterial);

        Collider *collider = new Collider{Sphere{Vec3(0.0), 1.0}, 1};
        RigidBody *rb = new RigidBody(mass, IBodySphere(radius, mass),
                0.9f, Vec3(0, -mass * gravity, 0), 0.1f, rollingFriction);
        rb->typeFriction = TypeFriction::rollingFriction;
        Object ball = newDynamicBody<MovingBall>(transform, model, collider, rb);
        auto& s = ball.AddSound(SoundType::SOUND_3D, "beat3.wav").SetVolume(0.5f).SetRadius(20.f);
        s.Start();
        s.Pause();
        return ball;
    }

    void Update(float dt) override {
        auto balls = self.CollideAll();
        for (auto ball : balls) {
            if (ball.GetCollider()->shape.index() == 1) {
                self.GetSound()->SetVolume(log2f(self.GetRigidBody()->velocity.length()) * 0.15f);
                self.GetSound()->Start();
                return;
            }
        }
        for (auto ball : balls) {
            if (ball.GetName() == "wall") {
                self.GetSound()->SetVolume(log2f(self.GetRigidBody()->velocity.length()) * 0.12f);
                self.GetSound()->Start();
                return;
            }
        }
    }
};

class Cue : public Behaviour {
 public:
    static Object New(std::vector<Object> objects, Camera *camera) {
        Model *model = engine->GetModelManager().LoadModel("pool/cue.obj");
        Material material = {
            4.f,
            Texture("/kiy.png"),
        };
        model->setMaterial(material);
        Transform *transform = new Transform(Vec3(0), Vec3(8), Mat4(0));
        Object newCue = newModel<Cue>(transform, model);
        newCue.AddAnimation();
        newCue.AddSound(SoundType::SOUND_FLAT, "beat3.wav").SetVolume(0.1f);
        reinterpret_cast<Cue*>(newCue.GetBehaviour())->Init(objects, camera);
        return newCue;
    }

    void Init(std::vector<Object> objects, Camera *camera) {
        m_Objects = objects;
        m_Camera = camera;
        m_CueDistance = 1.f;
        m_AttackVelocity = 12.f;
        m_Attacking = false;
    }

    void Update(float dt) override {
        Ray ray = Ray(m_Camera->GetPosition(), m_Camera->GetPosition() + m_Camera->GetFront());
        Object *target = nullptr;
        const float MAX_HIT_DIST = 1e9;
        float closest = MAX_HIT_DIST;
        for (int i = 0; i < m_Objects.size(); i++) {
            if (!m_Objects[i].IsValid()) continue;
            auto hit = m_Objects[i].GetCollider()->RaycastHit(*m_Objects[i].GetTransform(), ray);
            if (hit && *hit < closest) {
                target = &m_Objects[i];
                closest = *hit;
            }
        }

        if (s_Input->IsKeyPressed(Key::MouseLeft)) {
            if (m_CurrentTarget != nullptr) {
                m_Attacking = true;
                self.GetSound()->Start();
                Vec3 myPos = self.GetTransform()->GetTranslation();
                Vec3 targetPos = m_CurrentTarget->GetTransform()->GetTranslation();
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
                Vec3 targetPos = m_CurrentTarget->GetTransform()->GetTranslation();
                Vec3 direction = glm::normalize(targetPos - nextPos);
                direction.y = 0.f;
                m_CurrentTarget->GetRigidBody()->velocity += m_AttackVelocity * direction;
                m_CurrentTarget = nullptr;
            }
            m_Attacking = false;
        }

        if (m_Attacking)
            return;

        if (s_Input->IsKeyPressed(Key::MouseRight))
            m_CurrentTarget = nullptr;

        if (m_CurrentTarget != nullptr) {
            Vec3 center = m_CurrentTarget->GetTransform()->GetTranslation();
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
    Object *m_CurrentTarget = nullptr;
    std::vector<Object> m_Objects;
    Camera *m_Camera;
};

class GameManager : public Behaviour {
 private:
    PublicText *text;
    void ShowScore(int score) {
        text->SetContent("Score: " + std::to_string(score));
    }

 public:
    int score = 0;

    explicit GameManager(PublicText *text) {
        this->text = text;
    }

    void Update(float dt) override {
        ShowScore(score);
    }
};


class Hole : public TriggerArea {
 private:
    GameManager *gameManager;

 public:
    static Object New(Vec3 pos, GameManager *gm) {
        Object obj = engine->NewObject();
        obj.AddTransform(Transform(pos, Vec3(1), Mat4(1)));
        obj.AddCollider(Collider{AABB{Vec3(-0.25f), Vec3(0.25f)}});  // TO DO: replace with circle
        obj.AddBehaviour<Hole>(gm);
        return obj;
    }

    explicit Hole(GameManager *gm) {
        this->gameManager = gm;
    }

    void OnCollision(Object other) override {
        // Vec3 pos = self.GetTransform()->GetTranslation();
        // Logger::Info("hole: %f %f %f", pos.x, pos.y, pos.z);
        // check if some object is a ball and not a table or a player
        // i haven't found any way better than check transform parameters
        if (other.GetTransform()->GetScale().x < 1) {
            Consume(other);
        }
    }

    void Consume(Object ball) {
        gameManager->score += 100;
        ball.Remove();
    }
};

class Table : public Behaviour {
 public:
    static Object New(Vec3 position, Vec3 scale, GameManager *gameManager) {
        Transform *transform = new Transform(position, scale, Mat4(1.0));

        Model *model = engine->GetModelManager().LoadModel("pool/stol_1.obj");
        Material material = Material{
            4.f,
            Texture("pool/stol.png"),
        };
        model->setMaterial(material);

        // get full mesh of the table or make multiple objects for walls of the table.
        Collider *col = new Collider {&model->meshes[0], 1};
        float h0 = -0.5;
        float h = 0.85;

        float width = 0.9;
        float length = 0.45;

        /* Collider *col = new Collider{AABB { */
        /*     Vec3{-width, h0, -length}, */
        /*     Vec3{width, h, length}, */
        /* }}; */
        float floor_friction = 0.1f;
        float floor_bounciness = 0.05f;
        float walls_bounciness = 0.9f;
        Object obj = newStaticBody<Table>(transform, model, col, floor_bounciness, floor_friction);

        float wall_height = 0.3;
        /* AABB walls[] = { */
        /*     AABB {Vec3(width, h0, -length), Vec3(width + 0.01, h + wall_height, length)}, */
        /*     AABB {Vec3(-width - 0.01, h0, -length), Vec3(-width, h + wall_height, length)}, */
        /*     AABB {Vec3(-width, h0, length), Vec3(width, h + wall_height, length + 0.01)}, */
        /*     AABB {Vec3(-width, h0, -length - 0.01), Vec3(width, h + wall_height, -length)} */
        /* }; */
        /* for (int i = 0; i < 4; ++i) { */
        /*     Collider *col = new Collider{walls[i]}; */
        /*     newStaticBody(transform, col, walls_bounciness); */
        /* } */
        float top_y = position.y + 0.9f * scale.y;
        float table_w = scale.x * width * 2;
        float table_l = scale.z * length * 2;
        for (int i = 0; i <= 1; ++i) {
            for (int j = 0; j <= 2; ++j) {
                Vec3 pos = Vec3(table_w * (j - 1.0f) / 2.0f, top_y, table_l * (i - 0.5f));
                // Object hole = Hole::New(pos, gameManager);
                // model to show holes position (but not holes scale)
                // newModel(new Transform(pos, Vec3(0.1), Mat4(1)), Model::loadFromFile("cube.obj"));
            }
        }
        return obj;
    }

    void Update(float dt) override {}
};
