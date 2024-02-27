#include "object.hpp"
#include "engine.hpp"

Object::Object(Engine *engine, ObjectHandle id) : m_Engine(engine), m_Handle(id) {}
Object::Object() : m_Engine(nullptr), m_Handle(-1) {}

Object &Object::operator=(const Object &rhs) {
    m_Engine = rhs.m_Engine;
    m_Handle = rhs.m_Handle;
    return *this;
}

Object &Object::operator=(const Object &&rhs) {
    m_Engine = rhs.m_Engine;
    m_Handle = rhs.m_Handle;
    return *this;
}

Transform *Object::GetTransform() {
    return m_Engine->GetTransform(m_Handle);
}

RenderData *Object::GetRenderData() {
    return m_Engine->GetRenderData(m_Handle);
}

Collider *Object::GetCollider() {
    return m_Engine->GetCollider(m_Handle);
}

Animation *Object::GetAnimation() {
    return m_Engine->GetAnimation(m_Handle);
}

Text *Object::GetText() {
    return m_Engine->GetText(m_Handle);
}

PointLight *Object::GetPointLight() {
    return m_Engine->GetPointLight(m_Handle);
}

DirLight *Object::GetDirLight() {
    return m_Engine->GetDirLight(m_Handle);
}

SpotLight *Object::GetSpotLight() {
    return m_Engine->GetSpotLight(m_Handle);
}

Behaviour *Object::GetBehaviour() {
    return m_Engine->GetBehaviour(m_Handle);
}
