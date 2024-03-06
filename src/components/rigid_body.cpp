#include "rigid_body.hpp"
#include "logger.hpp"
#include "math.h"
#include "engine_config.hpp"
#include "user_config.hpp"
#include "collider.hpp"
#include <glm/gtx/string_cast.hpp>

Mat3 IBodySphere(float radius, float mass) {
    return Mat3(2.f/5 * mass * radius * radius);
}

RigidBody::RigidBody(float mass, Mat3 iBody, float restitution, Vec3 defaultForce,
         float kineticFriction) {
    SetMass(mass);
    SetIbodyInverse(iBody);
    this->restitution = restitution;
    this->defaultForce = defaultForce;
    this->kineticFriction = kineticFriction;
}

RigidBody::RigidBody(float mass, Mat3 iBody, Vec3 initalVelocity,
        float restitution, Vec3 defaultForce, Vec3 lineraUnlock,
        Vec3 angularUnlock, float kineticFriction) {
    SetMass(mass);
    SetIbodyInverse(iBody);
    this->velocity = initalVelocity;
    this->restitution = restitution;
    this->defaultForce = defaultForce;
    this->lineraUnlock = lineraUnlock;
    this->angularUnlock = angularUnlock;
    this->kineticFriction = kineticFriction;
}

void RigidBody::Update(Transform *tranform, float dt) {
    LinearCalculation(tranform, dt);
    AngularCalculation(tranform, dt);

    m_ResForce = defaultForce;
}

void RigidBody::ResolveCollisions(Transform tranform, Transform otherTransform,
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody,
        float dt) {
    ComputeForceTorque(tranform, otherTransform,
            collider, otherCollider, otherRigidBody, dt);
}

void RigidBody::SetMass(float mass) {
    if (mass == 0) {
        massInverse = 0;
        return;
    }
    massInverse = 1 / mass;
}

void RigidBody::SetIbodyInverse(Mat3 iBody) {
    if (iBody == Mat3(0)) {
        ibodyInverse = iBody;
        return;
    }
    ibodyInverse = glm::inverse(iBody);
}

void RigidBody::LinearCalculation(Transform *transform, float dt) {
    Vec3 acceleration = m_ResForce * massInverse;
    velocity += acceleration * dt;
    transform->Translate(velocity * lineraUnlock * dt);
}

void RigidBody::AngularCalculation(Transform *transform, float dt) {
    if (m_Torque == Vec3(0))
        return;
    Mat3 rotation = transform->GetRotation();
    Mat3 Iinverse = rotation * ibodyInverse * glm::transpose(rotation);
    Vec3 L = m_Torque * dt;
    Vec3 omega = Iinverse * L;
    auto angle = omega * angularUnlock * dt;
    transform->Rotate(angle.x, angle.y, angle.z);
}

void RigidBody::ApplyTorque(Vec3 force, Vec3 r) {
    m_Torque += glm::cross(force, r) * static_cast<float>(TORQUE_RATIO);
}

void RigidBody::ComputeFriction(Vec3 normalForce, float friction, Vec3 r) {
    Vec3 force = -Norm(velocity) * glm::length(friction * normalForce);

    m_ResForce += force;
    ApplyTorque(force, r);
}

Vec3 ImpulseForce(RigidBody *rigidBody, RigidBody *otherRigidBody,
    Vec3 normal, float velAlongNormal, float dt) {
    // Calculate restitution
    float e = std::min(rigidBody->restitution, otherRigidBody->restitution);
    // Calculate impulse scalar
    float impulseScalar = -(1 + e) * velAlongNormal;
    impulseScalar /= rigidBody->massInverse + otherRigidBody->massInverse;
    // Apply impulse
    Vec3 impulse = impulseScalar * normal;
    return impulse / dt;
}

Vec3 CollisionNormal(Collider *coll1, Collider *coll2,
    Transform tranform1, Transform tranform2, Vec3 vel, float dt) {
    return std::visit([=](auto var1) {
            return std::visit(
                [=](auto var2) {
                    return CollisionNormal(
                            var1, var2, tranform1, tranform2, vel, dt);
                }, coll2->shape);
        }, coll1->shape);
}

void RigidBody::ComputeForceTorque(Transform tranform, Transform otherTransform,
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody,
        float dt) {
    if (massInverse == 0 && otherRigidBody->massInverse == 0) {
        Logger::Error(
                "RIGID_BODY::CALC_IMPULSE_FORCE::BOTH_RIGID_BODIES_HAVE_INFINITY_MASS");
        return;
    }
    Vec3 rv = velocity - otherRigidBody->velocity;

    Vec3 normal = CollisionNormal(collider, otherCollider,
        tranform, otherTransform, rv, dt);

    float velAlongNormal = glm::dot(rv, normal);

    if (velAlongNormal > 0)
        return;

    Vec3 impulseForce = ImpulseForce(this, otherRigidBody, normal,
            velAlongNormal, dt);
    Vec3 normalForce = -m_ResForce * normal;
    Vec3 otherNormalForce = otherRigidBody->m_ResForce * normal;

    // Compute impulse
    m_ResForce += impulseForce;
    otherRigidBody->m_ResForce -= impulseForce;

    // Compute torque
    auto r1 = normal * tranform.GetScale() * 0.5f;
    ApplyTorque(-impulseForce, r1);
    auto r2 = otherTransform.GetTranslation()
        + normal * otherTransform.GetScale() * 0.5f;
    otherRigidBody->ApplyTorque(impulseForce, r2);

    // Compute normal force
    m_ResForce += normalForce;
    otherRigidBody->m_ResForce += otherNormalForce;

    // Compute friction
    if (velAlongNormal == 0) {
        auto friction = std::max(kineticFriction,
                otherRigidBody->kineticFriction);
        ComputeFriction(normalForce, friction, r1);
        otherRigidBody->ComputeFriction(otherNormalForce, friction, r2);
    }
}
