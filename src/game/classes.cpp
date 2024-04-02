#pragma once

#include <limits>
#include "engine.hpp"
#include "object.hpp"
#include "behaviour.hpp"
#include "logger.hpp"

extern Engine *engine;
extern ShaderProgram *defaultSP;

template<typename T>
Object newModel(Transform *transform, Model *model) {
    Object obj = engine->NewObject();
    obj.AddTransform(*transform);
    model->shader = defaultSP;
    obj.AddModel(*model);
    obj.AddBehaviour<T>();
    return obj;
}

Object newModel(Transform *transform, Model *model) {
    Object obj = engine->NewObject();
    obj.AddTransform(*transform);
    model->shader = defaultSP;
    obj.AddModel(*model);
    return obj;
}

template<typename T>
Object newStaticBody(Transform *transform, Model *model, Collider *collider) {
    Object obj = newModel<T>(transform, model);
    obj.AddCollider(*collider);
    obj.AddRigidBody(0.0f, Mat4(0), Vec3(0), 0.0f, Vec3(0), Vec3(1), Vec3(1), 0.0f);
    return obj;
}

Object newStaticBody(Transform *transform, Collider *collider) {
    Object obj = engine->NewObject();
    obj.AddTransform(*transform);
    obj.AddCollider(*collider);
    obj.AddRigidBody(0.0f, Mat4(0), Vec3(0), 0.0f, Vec3(0), Vec3(1), Vec3(1), 0.0f);
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
