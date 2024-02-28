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

RigidBody::RigidBody(float mass, Mat3 iBody, Vec3 initalVelocity,
        float restitution, Vec3 defaultForce, Vec3 defaultTorque,
        Vec3 lineraUnlock, Vec3 angularUnlock) {
    Mass(mass);
    IbodyInverse(iBody);
    this->velocity = initalVelocity;
    this->restitution = restitution;
    this->defaultForce = defaultForce;
    this->defaultTorque = defaultTorque;
    this->lineraUnlock = lineraUnlock;
    this->angularUnlock = angularUnlock;
}

void RigidBody::Update(Transform *tranform, float dt) {
    LinearCalculation(tranform, dt);
    AngularCalculation(tranform, dt);

    m_ResForce = defaultForce;
    m_Torque = defaultTorque;
}

void RigidBody::ResolveCollisions(Transform tranform, Transform otherTransform,
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody,
        float dt) {
    Compute(tranform, otherTransform,
            collider, otherCollider, otherRigidBody, dt);
}

void RigidBody::Mass(float mass) {
    if (mass == 0) {
        massInverse = 0;
        return;
    }
    massInverse = 1 / mass;
}

void RigidBody::IbodyInverse(Mat3 iBody) {
    if (iBody == Mat3(0)) {
        ibodyInverse = iBody;
        return;
    }
    ibodyInverse = glm::inverse(iBody);
}

void RigidBody::LinearCalculation(Transform *transform, float dt) {
    Vec3 acceleration = m_ResForce * massInverse;
    velocity += acceleration * dt;
    transform->Translate(velocity * lineraUnlock);
}

void RigidBody::AngularCalculation(Transform *transform, float dt) {
    if (m_Torque == Vec3(0))
        return;
    Mat3 rotation = transform->GetRotation();
    Mat3 Iinverse = rotation * ibodyInverse * glm::transpose(rotation);
    Vec3 L = m_Torque * dt;
    Vec3 omega = Iinverse * L;
    transform->RotateOmega(omega * angularUnlock);
}

void RigidBody::CalcTorque(Vec3 force, Vec3 r) {
    m_Torque += glm::cross(force, r);
}

Vec3 closetPoint(Vec3 point, Collider *collider, Transform tranform) {
    switch (collider->shape.index()) {
    case 0:
        return std::get<AABB>(collider->shape)
            .Transformed(tranform).ClosestPoint(point);
    case 1:
        return std::get<Sphere>(collider->shape)
            .Transformed(tranform).ClosestPoint(point);
    case 2:
        return std::get<Model*>(collider->shape)->
            ClosestPoint(point, tranform);
    }
    Logger::Error("RIGID_BODY::CLOSET_POINT::FALIE_TO_RETURN_SHAPE");
    return Vec3(0);
}

Vec3 collisionNormal(Collider *coll1, Collider *coll2,
    Transform tranform1, Transform tranform2, Vec3 vel, float dt) {
    return std::visit([=](auto var1) {
            return std::visit(
                [=](auto var2) {
                    return CollisionNormal(
                            var1, var2, tranform1, tranform2, vel, dt);
                }, coll2->shape);
        }, coll1->shape);
}

void RigidBody::Compute(Transform tranform, Transform otherTransform,
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody,
        float dt) {
    if (massInverse == 0 && otherRigidBody->massInverse == 0) {
        Logger::Error(
                "RIGID_BODY::CALC_IMPULSE_FORCE::BOTH_RIGID_BODIES_HAVE_INFINITY_MASS");
        return;
    }
    Vec3 rv = velocity - otherRigidBody->velocity;

    Vec3 normal = collisionNormal(collider, otherCollider,
        tranform, otherTransform, rv, dt);

    float velAlongNormal = glm::dot(rv, normal);

    if (velAlongNormal > 0)
        return;

    // Calculate restitution
    float e = std::min(restitution, otherRigidBody->restitution);
    // Calculate impulse scalar
    float impulseScalar = -(1 + e) * velAlongNormal;
    impulseScalar /= massInverse + otherRigidBody->massInverse;
    // Apply impulse
    Vec3 impulse = impulseScalar * normal;
    Vec3 force = impulse / dt;

    auto r1 = closetPoint(otherTransform.GetTranslation() - rv * dt,
            collider, tranform) - tranform.GetTranslation();
    CalcTorque(-force, r1);
    auto r2 = closetPoint(tranform.GetTranslation() + rv * dt,
            otherCollider, otherTransform) - otherTransform.GetTranslation();
    otherRigidBody->CalcTorque(force, r2);

    m_ResForce -= m_ResForce * normal;
    otherRigidBody->m_ResForce += otherRigidBody->m_ResForce * normal;

    m_ResForce += force;
    otherRigidBody->m_ResForce -= force;
}

