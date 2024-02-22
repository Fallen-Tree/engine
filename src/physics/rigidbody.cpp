#include "rigidbody.hpp"
#include "logger.hpp"
#include "math.h"
#include "engine_config.hpp"
#include "user_config.hpp"

 RigidBody::RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, 
         TypeRigidBody type, float restitution) {
    if (mass == 0) {
        m_MassInverse = 0;
    } else {
        m_MassInverse = 1 / mass;
    }
    m_IbodyInverse = glm::inverse(Ibody);
    m_Type= type;
    m_Velocity = initalVelocity;
    m_Restitution = restitution;
}

RigidBody::RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, TypeRigidBody type, 
    float restitution, Vec3 defaultForce) {
    Mass(mass);
    m_IbodyInverse = glm::inverse(Ibody);
    m_Type= type;
    m_Velocity = initalVelocity;
    m_Restitution = restitution;
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
            AngualarCalculation(tranform, dt);
            break;
        case LINEAR_ANGULAR: 
            AngualarCalculation(tranform, dt);  
            LinearCalculation(tranform, dt);
            break;
        case NONE:
            LinearCalculation(tranform, dt); // TODO:: delete this
            break;
        default:
            Logger::Error("RIGITBODY::UPDATE::UNKNOWN_TYPE_OF_RIGIDBODY"); 
            break;
    }
    m_ResForce = Vec3(0);
    m_Torque = Vec3(0);
}

void RigidBody::ResolveCollisions(RigidBody *otherRigidBody, float dt) {
    switch (m_Type) {
        case LINEAR:
            CalcForce(otherRigidBody, dt);
            break;
        case ANGULAR:
            CalcTorque(otherRigidBody, dt);
            break;
        case LINEAR_ANGULAR:
            CalcForce(otherRigidBody, dt);
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

void RigidBody::AddInstantaneousForce(Vec3 force) {
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
    transform->Translate(m_Velocity * float(0.01));
}

Mat4 calcRotMat(Vec3 omega, Mat3 rotation) {
    return Mat4(1); // TODO:: this block
}

void RigidBody::AngualarCalculation(Transform *transform, float dt) {
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
    //m_ResForce += DOWN * float(GRAVITATIONAL_ACCELERATION) * m_Mass;
    // m_ResForce -= 0.1f * m_Velocity * m_Velocity;
}

void RigidBody::CalcForce(RigidBody *otherRigidBody, float dt) {
    /*
    m_ResForce += otherRigidBody->m_Mass * otherRigidBody->m_Velocity 
        * otherRigidBody->impulseСoeffGive / dt;
    m_ResForce -= m_Mass * m_Velocity * otherRigidBody->impulseСoeffTake / dt;
    // changes force = m * V * dt * coeff
    //
    */
    Vec3 rv = m_Velocity - otherRigidBody->m_Velocity;

    Vec3 normal = Vec3(1); // TODO:: calculate noraml collision

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

