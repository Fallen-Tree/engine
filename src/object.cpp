#include "object.hpp"
#include "engine.hpp"

Object::Object(Engine *engine, ObjectHandle id) : m_Engine(engine), m_Handle(id) {}
Object::Object() : m_Engine(nullptr), m_Handle(-1) {}

void Object::Remove() {
    m_Engine->RemoveObject(m_Handle);
    m_Handle = -1;
}

bool Object::IsValid() {
    return m_Engine && m_Handle != -1;
}

void Object::AddChild(Object child) {
    m_Engine->AddChild(m_Handle, child.m_Handle);
}

Object Object::GetParent() {
    return m_Engine->GetParent(m_Handle);
}

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

Model *Object::GetModel() {
    return m_Engine->GetModel(m_Handle);
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

RigidBody *Object::GetRigidBody() {
    return m_Engine->GetRigidBody(m_Handle);
}

bool Object::Collide(Object other) {
    return m_Engine->Collide(m_Handle, other.m_Handle);
}

std::vector<Object> Object::CollideAll() {
    return m_Engine->CollideAll(m_Handle);
}
