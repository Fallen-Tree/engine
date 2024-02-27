#include "rigidbody.hpp"
#include "logger.hpp"
#include "math.h"
#include "engine_config.hpp"
#include "user_config.hpp"
#include "collider.hpp"
#include <glm/gtx/string_cast.hpp>

Mat3 IBodySphere(float radius, float mass) {
    return Mat3(2.f/5 * mass * radius * radius);
}

RigidBody::RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, 
        float restitution, Vec3 defaultForce, Vec3 defaultTorque,
        Vec3 lineraUnlock, Vec3 angularUnlock) { 
    Mass(mass);
    this->ibodyInverse = glm::inverse(Ibody);
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
    //Logger::Info("collision");
    CalcImpulseForce(tranform, otherTransform, 
            collider, otherCollider, otherRigidBody, dt);
}

void RigidBody::Mass(float mass) {
    if (mass == 0) {
        massInverse = 0;
    } else {
        massInverse = 1 / mass;
    }
}

void RigidBody::LinearCalculation(Transform *transform, float dt) {
    //Logger::Info("res force: %s", glm::to_string(m_ResForce).c_str());
    Vec3 acceleration = m_ResForce * massInverse;
    velocity += acceleration * dt;
    transform->Translate(velocity * lineraUnlock);
}

void RigidBody::AngularCalculation(Transform *transform, float dt) {
    if (m_Torque == Vec3(0))
        return;
    //Logger::Info("Here");
    Logger::Info("Torque : %s", glm::to_string(m_Torque).c_str());
    Mat3 rotation = transform->GetRotation();
    //Logger::Info("rotation: %s", glm::to_string(transform->GetRotation()).c_str());
    Mat3 Iinverse = rotation * ibodyInverse * glm::transpose(rotation);
    //Logger::Info("Iinverse: %s", glm::to_string(Iinverse).c_str());
    Vec3 L = m_Torque * dt; 
    //Logger::Info("L: %s", glm::to_string(L).c_str());
    Vec3 omega = Iinverse * L;
    //Logger::Info("omega: %s", glm::to_string(omega).c_str());
    transform->RotateOmega(omega * angularUnlock);
    //Logger::Info("rotation: %s", glm::to_string(transform->GetRotation()).c_str());
}

void RigidBody::CalcTorque(Vec3 force, Vec3 vec) {
    m_Torque += glm::dot(force, vec);
}

/*
Vec3 closetPoint(Vec3 point, Collider collider, Transform tranform,
        Vec3 veloctiy, float dt) {
    switch (collider.shape.index()) {
    case 0:
        AABB a = std::get<AABB>(collider.shape);
        return a.Transformed(tranform)
            .PrevState(veloctiy, dt).ClosestPoint(point);
    case 1:
        Sphere sphere = std::get<Sphere>(collider.shape);
        return sphere.Transformed(tranform)
            .PrevState(veloctiy, dt).ClosestPoint(point);
    case 2:
        Model *model = std::get<Model*>(collider.shape);
        return model->ClosestPoint(point, tranform);
    }
    Logger::Error("RIGID_BODY::CLOSET_POINT::FALIE_TO_RETURN_SHAPE");
    return Vec3(0);
}
*/

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

void RigidBody::CalcImpulseForce(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt) {
    if (massInverse == 0 && otherRigidBody->massInverse == 0) {
        Logger::Error(
                "RIGID_BODY::CALC_IMPULSE_FORCE::BOTH_RIGID_BODIES_HAVE_INFINITY_MASS"); 
        return;
    }
    Vec3 rv = velocity - otherRigidBody->velocity;

    Logger::Info("veloctiy: %s", glm::to_string(rv).c_str());
    //Logger::Info("point: %s", glm::to_string(tranform.GetTranslation()).c_str());
    Vec3 normal = collisionNormal(collider, otherCollider,
        tranform, otherTransform, rv, dt);
    Logger::Info("norm : %s", glm::to_string(normal).c_str());

    float velAlongNormal = glm::dot(rv, normal);

    Logger::Info("dot : %f", velAlongNormal);

    if (velAlongNormal > 0)
        return;

    // Calculate restitution
    float e = std::min(restitution, otherRigidBody->restitution);
    // Calculate impulse scalar
    float impulseScalar = -(1 + e) * velAlongNormal;
    impulseScalar /= massInverse + otherRigidBody->massInverse;
    // Apply impulse
    Vec3 impulse = impulseScalar * normal;
    Logger::Info("impulse : %s", glm::to_string(impulse).c_str());

    Vec3 force = impulse / dt;

    CalcTorque(force, tranform.GetScale() * normal);

    Logger::Info("impulse: %s", glm::to_string(impulse).c_str());
    m_ResForce -= m_ResForce * normal; 
    otherRigidBody->m_ResForce += otherRigidBody->m_ResForce * normal;

    m_ResForce += force;
    otherRigidBody->m_ResForce -= force;
}

