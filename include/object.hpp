#pragma once

#include <string>
#include "transform.hpp"
#include "collider.hpp"
#include "animation.hpp"
#include "text.hpp"
#include "light.hpp"
#include "render_data.hpp"
#include "engine.hpp"

class Object {
 public:
    int name = 0;

    Object(const Object &) = default;
    Object(Object &&) = default;
    Object& operator=(const Object &);
    Object& operator=(const Object &&);
    Object();
    Object(Engine *, ObjectHandle);

    void SetEngine(Engine *);

    Transform *GetTransform();
    Model *GetModel();
    Collider *GetCollider();
    Animation *GetAnimation();
    SkeletalAnimationsManager *GetSkeletalAnimationsManager();
    Text *GetText();
    PointLight *GetPointLight();
    DirLight *GetDirLight();
    SpotLight *GetSpotLight();
    Behaviour *GetBehaviour();
    RigidBody *GetRigidBody();
    Sound *GetSound();

    template<typename ...Ts>
    Transform &AddTransform(Ts... ts) {
        return m_Engine->AddTransform(m_Handle, Transform{ts...});
    }

    template<typename ...Ts>
    Model &AddModel(Ts... ts) {
        return m_Engine->AddModel(m_Handle, Model{ts...});
    }

    template<typename ...Ts>
    Collider &AddCollider(Ts... ts) {
        return m_Engine->AddCollider(m_Handle, Collider{ts...});
    }

    template<typename ...Ts>
    RigidBody &AddRigidBody(Ts... ts) {
        return m_Engine->AddRigidBody(m_Handle, RigidBody{ts...});
    }

    template<typename ...Ts>
    Text &AddText(Ts... ts) {
        return m_Engine->AddText(m_Handle, Text{ts...});
    }

    template<typename ...Ts>
    SkeletalAnimationsManager &AddSkeletalAnimationsManager(Ts... ts) {
        return m_Engine->AddSkeletalAnimationsManager(m_Handle, SkeletalAnimationsManager{ts...});
    }

    template<typename ...Ts>
    Image &AddImage(Ts... ts) {
        return m_Engine->AddImage(m_Handle, Image{ts...});
    }

    template<typename ...Ts>
    Sound &AddSound(Ts... ts) {
        return m_Engine->AddSound(m_Handle, Sound{ts...});
    }

    template<typename ...Ts>
    Animation &AddAnimation(Ts... ts) {
        return m_Engine->AddAnimation(m_Handle, Animation{ts...});
    }

    template<typename ...Ts>
    SpotLight &AddSpotLight(Ts... ts) {
        return m_Engine->AddSpotLight(m_Handle, SpotLight{ts...});
    }

    template<typename ...Ts>
    PointLight &AddPointLight(Ts... ts) {
        return m_Engine->AddPointLight(m_Handle, PointLight{ts...});
    }

    template<typename ...Ts>
    DirLight &AddDirLight(Ts... ts) {
        return m_Engine->AddDirLight(m_Handle, DirLight{ts...});
    }

    template<typename T, typename ...Ts>
    T &AddBehaviour(Ts... ts) {
        auto &res = m_Engine->AddBehaviour(m_Handle, new T{ts...});
        res.self = *this;
        return res;
    }

    void Remove();
    bool IsValid();
    void AddChild(Object child);
    Object GetParent();
    bool Collide(Object other);
    std::vector<Object> CollideAll();

 private:
    Engine *m_Engine;
    ObjectHandle m_Handle;
};
