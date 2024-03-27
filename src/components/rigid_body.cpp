#include "rigid_body.hpp"
#include "logger.hpp"
#include "time.hpp"
#include "pretty_print.hpp"
#include "engine_config.hpp"
#include "user_config.hpp"
#include "collider.hpp"

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
    this->linearUnlock = lineraUnlock;
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
    transform->Translate(velocity * linearUnlock * dt);
}

void RigidBody::AngularCalculation(Transform *transform, float dt) {
    if (m_Torque == Vec3(0))
        return;
    Mat3 rotation = transform->GetRotation();
    Mat3 Iinverse = rotation * ibodyInverse * glm::transpose(rotation);
    Vec3 L = m_Torque * dt;
    Vec3 omega = Iinverse * L;
    auto angle = omega * angularUnlock;
    transform->Rotate(angle.x, angle.y, angle.z);
}

void RigidBody::ApplyTorque(Vec3 force, Vec3 r) {
    m_Torque += glm::cross(force, r) * static_cast<float>(TORQUE_RATIO);
}

void RigidBody::LimitTorque(Vec3 force, Vec3 r) {
    Vec3 torque = glm::cross(force, r) * static_cast<float>(TORQUE_RATIO);
    m_Torque = TORQUE_SMOTHNESS * m_Torque + (1 - TORQUE_SMOTHNESS) * torque;
}

Vec3 CalculateFrictionDirection(Vec3 normal, Vec3 velocity) {
    Vec3 res = Norm(velocity - Norm(normal) * glm::dot(velocity, Norm(normal)));
    return res;
}

void RigidBody::ComputeFriction(Vec3 normalForce, float friction,
        Vec3 r, float dt, Vec3 normal) {
    if (massInverse == 0)
        return;
    Vec3 direction = CalculateFrictionDirection(normal, velocity);
    // Full friction force
    Vec3 force = -direction * glm::length(friction * normalForce);

    // if result force in direction of veloctiy is less than full friction
    // force, than rigid bodu will stop.
    if (glm::length(m_ResForce * direction
            + ((direction * velocity) /(dt * massInverse)))
            < glm::length(force)) {
        //Logger::Info("here");
        //Logger::Info("mass %f", massInverse);
        //Logger::Info("vel %s", ToString(velocity));
        //Logger::Info("force %s", ToString(m_ResForce));
        //Logger::Info("minus %s", ToString(-velocity * glm::abs(direction)));
        m_ResForce -= m_ResForce * glm::abs(direction);
        velocity -= velocity * glm::abs(direction);
        //Logger::Info("vel %s", ToString(velocity));
        //Logger::Info("force %s", ToString(m_ResForce));
        //Logger::Info("mass %0.15f", velocity.x);
        LimitTorque(force, r);
        return;
    }

    Logger::Info("here2");
    Logger::Info("vel %s", ToString(velocity));
    Logger::Info("mass %f", massInverse);
    // otherwise apply full friction force
    m_ResForce += force;
    LimitTorque(force, r);
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
    return (impulse / dt);
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
    Logger::Info("mass %f", massInverse);
    Logger::Info("mass2 %f", otherRigidBody->massInverse);
    Vec3 rv = velocity - otherRigidBody->velocity;

    Vec3 normal = CollisionNormal(collider, otherCollider,
        tranform, otherTransform, rv, dt);

    Logger::Info("normal %s", ToString(normal));

    float velAlongNormal = glm::dot(rv, normal);

    Vec3 impulseForce = ImpulseForce(this, otherRigidBody, normal,
            velAlongNormal, dt);
    Logger::Info("impulse %s", ToString(impulseForce));

    // Compute normal force
    Logger::Info("force %s", ToString(m_ResForce));
    Logger::Info("force2 %s", ToString(otherRigidBody->m_ResForce));
    Logger::Info("vlocity %s", ToString(velocity));
    Logger::Info("velocity2 %s", ToString(otherRigidBody->velocity));
    Vec3 normalForce = -m_ResForce * normal;
    Vec3 otherNormalForce = otherRigidBody->m_ResForce * normal;
    Logger::Info("force %s", ToString(m_ResForce));
    Logger::Info("force2 %s", ToString(otherRigidBody->m_ResForce));
    Logger::Info("unlock %s", ToString(linearUnlock));
    Logger::Info("unlock2 %s", ToString(otherRigidBody->linearUnlock));
    m_ResForce += normalForce;
    otherRigidBody->m_ResForce += otherNormalForce;


    velocity -= velocity * glm::abs(normal);
       //* (Vec3(1) - otherRigidBody->linearUnlock);
    otherRigidBody->velocity -= otherRigidBody->velocity * glm::abs(normal);
        //* (Vec3(1) - linearUnlock);
    Logger::Info("min %s", ToString(otherRigidBody->velocity * glm::abs(normal) * linearUnlock));
    /*if (otherRigidBody->massInverse == 0
            || massInverse > otherRigidBody->massInverse) {
        velocity -= velocity * glm::abs(normal);
    }
    if (massInverse == 0
            || massInverse < otherRigidBody->massInverse) {
        otherRigidBody->velocity -= otherRigidBody->velocity * glm::abs(normal);
    }
    */

    Logger::Info("velocity %s", ToString(velocity));
    Logger::Info("velocity %s", ToString(velocity));
    Logger::Info("velocity2 %s", ToString(otherRigidBody->velocity));
    Logger::Info("force %s", ToString(m_ResForce));
    Logger::Info("force2 %s", ToString(otherRigidBody->m_ResForce));

    // Compute lever of force
    auto r1 = normal * tranform.GetScale() * 0.5f;
    auto r2 = -normal * otherTransform.GetScale() * 0.5f;

    // Compute impulse
    if (velAlongNormal < 0) {
        Logger::Info("impulse");
        m_ResForce += impulseForce;
        otherRigidBody->m_ResForce -= impulseForce;

        // Compute torque for impulse force
        ApplyTorque(-impulseForce, r1);
        otherRigidBody->ApplyTorque(impulseForce, r2);
    }

    // Compute friction
    auto friction = (kineticFriction + otherRigidBody->kineticFriction) / 2;
    ComputeFriction(normalForce, friction, r1, dt, normal);
    otherRigidBody->ComputeFriction(otherNormalForce, friction, r2, dt, -normal);
}
