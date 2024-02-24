#pragma once
#include "transform.hpp"
#include "collider.hpp"
#include "animation.hpp"
#include "text.hpp"
#include "light.hpp"
#include "render_data.hpp"
#include "engine.hpp"

class Object {
 public:
    Object(Engine &, ObjectHandle);

    Transform *GetTransform();
    RenderData *GetRenderData();
    Collider *GetCollider();
    Animation *GetAnimation();
    Text *GetText();
    PointLight *GetPointLight();
    DirLight *GetDirLight();
    SpotLight *GetSpotLight();
    Behaviour *GetBehaviour();

    template<typename ...Ts>
    Transform &AddTransform(Ts... ts) {
        return m_Engine.AddTransform(m_Handle, Transform{ts...});
    }

    template<typename ...Ts>
    RenderData &AddRenderData(Ts... ts) {
        return m_Engine.AddRenderData(m_Handle, RenderData{ts...});
    }

    template<typename ...Ts>
    Collider &AddCollider(Ts... ts) {
        return m_Engine.AddCollider(m_Handle, Collider{ts...});
    }

    template<typename ...Ts>
    Text &AddText(Ts... ts) {
        return m_Engine.AddText(m_Handle, Text{ts...});
    }

    template<typename ...Ts>
    Animation &AddAnimation(Ts... ts) {
        return m_Engine.AddAnimation(m_Handle, Animation{ts...});
    }

    template<typename ...Ts>
    SpotLight &AddSpotLight(Ts... ts) {
        return m_Engine.AddSpotLight(m_Handle, SpotLight{ts...});
    }

    template<typename ...Ts>
    PointLight &AddPointLight(Ts... ts) {
        return m_Engine.AddPointLight(m_Handle, PointLight{ts...});
    }

    template<typename ...Ts>
    DirLight &AddDirLight(Ts... ts) {
        return m_Engine.AddDirLight(m_Handle, DirLight{ts...});
    }

 private:
    Engine &m_Engine;
    ObjectHandle m_Handle;
};
