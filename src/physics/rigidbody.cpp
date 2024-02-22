#include "rigidbody.hpp"
#include "logger.hpp"
#include "math.h"
#include "engine_config.hpp"
#include "user_config.hpp"
#include "collider.hpp"

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

void RigidBody::ResolveCollisions(Transform *tranform, 
        Transform *otherTransform, RigidBody *otherRigidBody, 
        Collider *otherColider, float dt) {
    switch (m_Type) {
        case LINEAR:
            CalcForce(tranform, otherTransform, otherRigidBody, 
                    otherColider, dt);
            break;
        case ANGULAR:
            CalcTorque(otherRigidBody, dt);
            break;
        case LINEAR_ANGULAR:
            CalcForce(tranform, otherTransform, otherRigidBody, 
                    otherColider, dt);
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

Vec3 calcCollisionNormal(Vec3 point, Transform *otherTransform,
        std::variant<AABB, Sphere, Model *> var) {
    Vec3 collisionPoint;
    Logger::Info("index : %d", var.index());
    switch (var.index()) {
    case 0:
        collisionPoint = std::get<AABB>(var).ClosestPoint(point);
        break;
    case 1:
        collisionPoint = std::get<Sphere>(var).ClosestPoint(point);
        break;
    case 2:
        collisionPoint = std::get<Model*>(var)->
            ClosestPoint(point, otherTransform);
        break;
    default:
        Logger::Error("RIGID_BODY::CALCFORCE::UNDEFINED_TYPE_OF_SHAPE");
        break;
    }
    return glm::normalize(collisionPoint - point);
}

void RigidBody::CalcForce(Transform *tranform, Transform *otherTransform, 
        RigidBody *otherRigidBody, Collider *otherColider, float dt) {
    Vec3 rv = m_Velocity - otherRigidBody->m_Velocity;

    Vec3 normal = calcCollisionNormal(tranform->GetTranslation(),
            otherTransform, otherColider->shape);

    float velAlongNormal = glm::dot(rv, normal);

    if (velAlongNormal > 0)
        return;
    
    // Calculate restitution
    float e = std::min(m_Restitution, otherRigidBody->m_Restitution);
    // Calculate impulse scalar
    float impulseScalar = -(1 + e) * velAlongNormal;
    impulseScalar /= m_MassInverse + otherRigidBody->m_MassInverse;
    // Apply impulse
    Vec3 impulse = impulseScalar * normal;

    m_ResForce -= impulse * dt;
    otherRigidBody->m_ResForce += impulse * dt;
}

