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
    obj.AddModel(*model);
    obj.AddBehaviour<T>();
    return obj;
}

Object newModel(Transform *transform, Model *model) {
    Object obj = engine->NewObject();
    obj.AddTransform(*transform);
    obj.AddModel(*model);
    return obj;
}

template<typename T>
Object newStaticBody(Transform *transform, Model *model, Collider *collider,
        float bounciness = 0.2f, float friction = 0.5f) {
    Object obj = newModel<T>(transform, model);
    obj.AddCollider(*collider);
    obj.AddRigidBody(0.0f, Mat4(0), bounciness, Vec3(0), friction, slidingFriction);
    return obj;
}

Object newStaticBody(Transform *transform, Model *model, Collider *collider,
        float bounciness = 0.2f, float friction = 0.5f) {
    Object obj = newModel(transform, model);
    obj.AddCollider(*collider);
    obj.AddRigidBody(0.0f, Mat4(0), bounciness, Vec3(0), friction, slidingFriction);
    return obj;
}


template<typename T>
Object newStaticBody(Transform *transform, Collider *collider,
        float bounciness = 0.2f, float friction = 0.5f) {
    Object obj = engine->NewObject();
    obj.AddTransform(*transform);
    obj.AddCollider(*collider);
    obj.AddRigidBody(0.0f, Mat4(0), bounciness, Vec3(0), friction, slidingFriction);
    return obj;
}

Object newStaticBody(Transform *transform, Collider *collider,
        float bounciness = 0.2f, float friction = 0.5f) {
    Object obj = engine->NewObject();
    obj.AddTransform(*transform);
    obj.AddCollider(*collider);
    obj.AddRigidBody(0.0f, Mat4(0), bounciness, Vec3(0), friction, slidingFriction);
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

Object newDynamicBody(Transform *transform, Model *model,
        Collider *collider, RigidBody *rigidBody) {
    Object obj = newModel(transform, model);
    obj.AddCollider(*collider);
    obj.AddRigidBody(*rigidBody);
    return obj;
}

class TriggerArea : public Behaviour {
 public:
    virtual void OnCollision(Object other) = 0;

    void Update(float dt) override {
        // I need a function too get all colliders (or objects) intersecting with this object
        std::vector<Object> colls = self.CollideAll();
        for (auto obj : colls) {
            OnCollision(obj);
        }
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
