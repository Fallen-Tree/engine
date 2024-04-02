#include <limits>
#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "logger.hpp"

// Made this global for easier usage inside functions
Engine *engine;

// I'd like to move responsibility about default shader to engine
ShaderProgram *defaultSP;

// Initializing global variables
void init() {
    engine = new Engine();
    std::string vertexShaderSource = "/standart.vshader";
    std::string fragmentShaderSource = "/standart.fshader";
    Shader vShader = Shader(VertexShader, vertexShaderSource);
    Shader fShader = Shader(FragmentShader, fragmentShaderSource);
    defaultSP = new ShaderProgram(vShader, fShader);
}

// My functions for object creation
template<typename T>
Object newModel(Transform *transform, Model *model) {
    Object obj = engine->NewObject();
    obj.AddTransform(*transform);
    model->shader = defaultSP;
    obj.AddModel(*model);
    obj.AddBehaviour<T>();
    return obj;
}

template<typename T>
Object newStaticBody(Transform *transform, Model *model, Collider *collider) {
    Object obj = newModel<T>(transform, model);
    obj.AddCollider(*collider);
    obj.AddRigidBody(0.0f, Mat4(0), Vec3(0), 3.0f, Vec3(0), Vec3(0), Vec3(0), 0.0001f);
    return obj;
}

Object newStaticBody(Transform *transform, Collider *collider) {
    Object obj = engine->NewObject();
    obj.AddTransform(*transform);
    obj.AddCollider(*collider);
    obj.AddRigidBody(0.0f, Mat4(0), Vec3(0), 3.0f, Vec3(0), Vec3(0), Vec3(0), 0.0001f);
    return obj;
}

template<typename T>
Object newDynamicBody(Transform *transform, Model *model,
        Collider *collider, RigidBody *rigidBody) {
    Object obj = newModel<T>(transform, model);
    obj.AddCollider(*collider);
    obj.AddRigidBody(*rigidBody);
    return obj;
}

class TriggerArea : public Behaviour {
 public:
    static Object New(Transform *transform, Collider *collider) {
        Object obj = engine->NewObject();
        obj.AddTransform(*transform);
        obj.AddCollider(*collider);
        // mark collider as trigger
        return obj;
    }

    virtual void OnCollision(Collider other);

    void Update(float dt) override {
        // I need a function too get all colliders (or objects) intersecting with this object
        // std::vector<Collider> colls = self.GetCollider()->getCollisions();
        // for (auto col : colls) {
        //     OnCollision(col);
        // }
    }
};

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
                Vec3(0), 0.6f, Vec3(0, -mass * gravity, 0), Vec3(1), Vec3(1), 0.001f);
        return newDynamicBody<MovingBall>(transform, model, collider, rb);
    }

    void Update(float dt) override {}
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

    void Consume(Object *ball) {
        // delete ball
        // add score
        // how???
    }

    // i hate that i need to override Update even if it is empty
    void Update(float dt) override {}
};

class PublicText : public Behaviour {
 public:
    static Object New(Font* font, std::string text, Vec2 position, float scale, Vec3 color) {
        Object obj = engine->NewObject();
        obj.AddText(font, text, position.x, position.y, scale, color);
        obj.AddBehaviour<PublicText>();
        return obj;
    }

    void SetContent(std::string text) {
        self.GetText()->SetContent(text);
    }

    void Update(float dt) override {}
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
        float closest = std::numeric_limits<float>::max();
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

void createUI() {
    class FpsText : public Behaviour {
     public:
        void Update(float dt) override {
            int fps = Time::GetCurrentFps();
            char buf[12];
            snprintf(buf, sizeof(buf), "Fps: %d", fps);
            self.GetText()->SetContent(buf);
        }
    };

    auto ocraFont = new Font("OCRAEXT.TTF", 20);
    auto obj = engine->NewObject();
    obj.AddText(ocraFont, "", 0.85f, 0.95f, 1.f, Vec3(0, 0, 0));
    // obj.AddText(ocraFont, "", 685.0f, 575.0f, 1.f, Vec3(0, 0, 0));
    obj.AddBehaviour<FpsText>();

    engine->NewObject().AddImage("hp.png", 0.03f, 0.15f, 0.4f);
    engine->NewObject().AddImage("hp_bar.png", 0.015f, 0.01f, 0.4f);
}

void createLights() {
    engine->NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(-0.2, -0.5, -1.2),
        1.f, 0.09f, 0.032f);

    engine->NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(2.3f, -3.3f, -4.0f),
        1.f, 0.09f, 0.032f);

    engine->NewObject().AddPointLight(
        Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f,  0.0f, -3.0f),
        1.f, 0.09f, 0.032f);

    engine->NewObject().AddDirLight(
        Vec3(0.05f, 0.05f, 0.05f), Vec3(0.4f, 0.4f, 0.4f),
        Vec3(0.5f, 0.5f, 0.5f),  Vec3(-0.2f, -1.0f, -0.3f));

    engine->NewObject().AddSpotLight(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f),
        Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f),
        1.0f, 0.09f, 0.032f, Vec3(0),
        glm::cos(glm::radians(12.5f)),
        glm::cos(glm::radians(15.0f)));
}

void addCat() {
    Model * model = Model::loadFromFile("cat.obj");
    model->shader = defaultSP;
    Material cat_material = {
        4.f,
        Texture("/Cat_diffuse.png", "/Cat_specular.png")
    };
    model->setMaterial(cat_material);
    auto obj = engine->NewObject();
    obj.AddModel(*model);
    auto &t = obj.AddTransform(Vec3(0.f, -3.f, -8.f), Vec3(.1f, .1f, .1f), Mat4(1.0));
    t.Rotate(1.67f, Vec3(-1.f, 0.f, 0.f));
    obj.AddCollider(Collider::GetDefaultAABB(&model->meshes[0]));
    // obj.AddRigidBody(100.f, Mat3(0), Vec3(0), 0.f, Vec3(0, -1000, 0),
    //     Vec3(0), Vec3(1), 0.1f);
}

int main() {
    // Logger::SetLoggingLevel(WARN);

    init();

    std::vector<Vec3> coordinates {
        Vec3(-2, -2.0, -0.2f),
        Vec3(-2.5f, -2.0, -0.5f),
        Vec3(-2.5f, -2.0, 0.1f),
        Vec3(-3.f, -2.0, -0.8f),
        Vec3(-3.f, -2.0, -0.2f),
        Vec3(-3.f, -2.0, 0.4f),
        Vec3(-3.5f, -2.0, -1.1f),
        Vec3(-3.5f, -2.0, -0.5f),
        Vec3(-3.5f, -2.0, 0.1f),
        Vec3(-3.5f, -2.0, 0.7f),
        Vec3(-4, -2.0, -1.4f),
        Vec3(-4, -2.0, -0.8f),
        Vec3(-4, -2.0, -0.2f),
        Vec3(-4, -2.0, 0.4f),
        Vec3(-4, -2.0, 1.f),

        Vec3(2, -2.0, -0.2f),
    };

    int ballsCount = coordinates.size();
    std::vector<MovingBall*> balls;
    for (int i = 0; i < ballsCount; i++) {
        Vec3 pos = coordinates[i];
        auto newBall = MovingBall::New(pos, 0.2, std::to_string(i % 16 + 1) + ".png");
        balls.push_back(reinterpret_cast<MovingBall*>(newBall.GetBehaviour()));
    }

    Table::New(Vec3(0, -7, 0), Vec3(5));

    Cue::New(balls, engine->camera);

    // addCat();

    createUI();

    createLights();

    engine->Run();
}
