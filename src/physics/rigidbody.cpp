#include "rigidbody.hpp"
#include "logger.hpp"
#include "math.h"
#include "engine_config.hpp"
#include "user_config.hpp"
#include "collider.hpp"
#include <glm/gtx/string_cast.hpp>

RigidBody::RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, TypeRigidBody type, 
        float restitution, Vec3 defaultForce, Vec3 defaultTorque) {
    Mass(mass);
    m_IbodyInverse = glm::inverse(Ibody);
    m_Type = type;
    m_Velocity = initalVelocity;
    m_Restitution = restitution;
    m_DefaultForce = defaultForce;
    m_DefaultTorque = defaultTorque;
}

void RigidBody::Compute_Force_Torque(float dt) {
    switch(m_Type) {
        case LINEAR:
            CalcForce(dt);
            break;
        case ANGULAR:
            CalcTorque(dt);
            break;
        case LINEAR_ANGULAR:
            CalcForce(dt);
            CalcTorque(dt);
            break;
        case NONE:
            break;
        default:
            Logger::Error("RIGITBODY::COMPUTE_FORCE_TORQUE::UNKNOWN_TYPE_OF_RIGIDBODY"); 
            break;
    }
}

void RigidBody::Update(Transform *tranform, float dt) {
    switch (m_Type) {
        case LINEAR:
            LinearCalculation(tranform, dt);
            break;
        case ANGULAR:
            AngularCalculation(tranform, dt);
            break;
        case LINEAR_ANGULAR: 
            AngularCalculation(tranform, dt);  
            LinearCalculation(tranform, dt);
            break;
        case NONE:
            break;
        default:
            Logger::Error("RIGITBODY::UPDATE::UNKNOWN_TYPE_OF_RIGIDBODY"); 
            break;
    }
    m_ResForce = m_DefaultForce;
    m_Torque = Vec3(0);
}

void RigidBody::ResolveCollisions(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt) {
    Logger::Info("collision");
    switch (m_Type) {
        case LINEAR:
            CalcForce(tranform, otherTransform, 
                collider, otherCollider, otherRigidBody, dt);
            break;
        case ANGULAR:
            CalcTorque(otherRigidBody, dt);
            break;
        case LINEAR_ANGULAR:
            CalcForce(tranform, otherTransform, 
                collider, otherCollider, otherRigidBody, dt);
            CalcTorque(otherRigidBody, dt);
            break;
        case NONE:
            break;
        default:
            Logger::Error(
                    "RIGITBODY::RESOLVE_COLLISIONS::UNKNOWN_TYPE_OF_RIGIDBODY"); 
            break;
    }
}

void RigidBody::AddVelocity(Vec3 velocity) {
    m_Velocity += velocity;
}

void RigidBody::AddInstantForce(Vec3 force) {
    m_DefaultForce += force;
}

void RigidBody::AddDefaultForce(Vec3 force) {
    m_DefaultForce += force;
}

void RigidBody::SetDefaultForce(Vec3 force) { 
    m_DefaultForce = force; 
    m_ResForce = force; 
}

void RigidBody::Mass(float mass) {
    if (mass == 0) {
        m_MassInverse = 0;
    } else {
        m_MassInverse = 1 / mass;
    }
}

void RigidBody::LinearCalculation(Transform *transform, float dt) {
    Logger::Info("res force: %s", glm::to_string(m_ResForce).c_str());
    Vec3 acceleration = m_ResForce * m_MassInverse;
    m_Velocity += acceleration * dt;
    transform->Translate(m_Velocity);
}

Mat4 calcRotMat(Vec3 omega, Mat3 rotation) {
    return Mat4(1); // TODO:: this block
}

void RigidBody::AngularCalculation(Transform *transform, float dt) {
    Mat3 rotation = transform->GetRotation();
    Mat3 Iinverse = rotation * m_IbodyInverse * glm::transpose(rotation);
    Vec3 L = m_Torque * dt; 
    Vec3 omega = Iinverse * L;
    transform->SetRotation(calcRotMat(omega, rotation));
}

void RigidBody::CalcTorque(float dt) {
    // TODO:: this block
}

void RigidBody::CalcTorque(RigidBody *otherRigidBody, float dt) {
    // TODO:: this block
}

void RigidBody::CalcForce(float dt) {
    // TODO:: this block
}

Vec3 calcCollisionNormal(Collider *coll1, Collider *coll2,
    Transform tranform1, Transform tranform2, Vec3 vel, float dt) {
    return std::visit([=](auto var1) {
            return std::visit(
                [=](auto var2) {
                    return CollisionNormal(
                            var1, var2, tranform1, tranform2, vel, dt);
                }, coll2->shape);
        }, coll1->shape);
}

void RigidBody::CalcForce(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt) {
    Vec3 rv = m_Velocity - otherRigidBody->m_Velocity;

    //Logger::Info("point: %s", glm::to_string(tranform->GetTranslation()).c_str());
    Vec3 normal = calcCollisionNormal(collider, otherCollider,
        tranform, otherTransform, rv, dt);
    Logger::Info("norm : %s", glm::to_string(normal).c_str());

    float velAlongNormal = glm::dot(rv, normal);

    Logger::Info("dot : %f", velAlongNormal);

    if (velAlongNormal > 0)
        return;

    // Calculate restitution
    float e = std::min(m_Restitution, otherRigidBody->m_Restitution);
    // Calculate impulse scalar
    float impulseScalar = -(1 + e) * velAlongNormal;
    impulseScalar /= m_MassInverse + otherRigidBody->m_MassInverse;
    // Apply impulse
    Vec3 impulse = impulseScalar * normal;
    Logger::Info("impulse : %s", glm::to_string(impulse).c_str());

    Logger::Info("impulse: %s", glm::to_string(impulse).c_str());
    //m_ResForce += m_ResForce * normal;
    //otherRigidBody->m_ResForce += otherRigidBody->m_ResForce * normal;
    m_ResForce += impulse / dt;
    otherRigidBody->m_ResForce -= impulse / dt;
}

