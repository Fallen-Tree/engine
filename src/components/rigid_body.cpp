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

Mat3 IBodyOBB(Vec3 halfWidth, float mass) {
    Mat3 res = Mat3(0);
    float d = halfWidth.x;
    float w = halfWidth.y;
    float h = halfWidth.z;
    res[0][0] = (w * w + h * h) * mass / 12.f;
    res[1][1] = (d * d + h * h) * mass / 12.f;
    res[2][2] = (w * w + d * d) * mass / 12.f;
    return res;
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
        float restitution, Vec3 defaultForce,
        Vec3 angularUnlock, float kineticFriction) {
    SetMass(mass);
    SetIbodyInverse(iBody);
    this->velocity = initalVelocity;
    this->restitution = restitution;
    this->defaultForce = defaultForce;
    this->angularUnlock = angularUnlock;
    this->kineticFriction = kineticFriction;
}

void RigidBody::Update(Transform *tranform, float dt) {
    LinearCalculation(tranform, dt);
    AngularCalculation(tranform, dt);

    m_ResForce = defaultForce;
}

void RigidBody::ResolveCollisions(RigidBody *otherRigidBody,
        CollisionManifold manifold, Transform transform,
        Transform otherTransform, float dt) {
    ComputeForceTorque(otherRigidBody, manifold,
            transform, otherTransform, dt);
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
    transform->Translate(velocity * dt);
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
    auto cross = glm::cross(force, r);
    Logger::Info("force %0.10f %0.10f %0.10f", force.x, force.y, force.z);
    Logger::Info("r %0.10f %0.10f %0.10f", r.x, r.y, r.z);
    Logger::Info("cross %0.10f %0.10f %0.10f", cross.x, cross.y, cross.z);
    m_Torque += glm::cross(force, r) * static_cast<float>(TORQUE_RATIO);
}

void RigidBody::LimitTorque(Vec3 force, Vec3 r) {
    Vec3 torque = glm::cross(force, r) * static_cast<float>(TORQUE_RATIO);
    m_Torque = TORQUE_SMOTHNESS * m_Torque + (1 - TORQUE_SMOTHNESS) * torque;
}

inline Vec3 CalculateFrictionDirection(Vec3 normal, Vec3 velocity) {
    Vec3 res = Norm(velocity - Norm(normal) * glm::dot(velocity, Norm(normal)));
    return res;
}

inline void RigidBody::ComputeFriction(Vec3 normalForce, float friction,
        Vec3 r, float dt, Vec3 normal) {
    if (massInverse == 0)
        return;
    Vec3 direction = CalculateFrictionDirection(normal, velocity);
    // Full friction force
    Vec3 frictionForce = -direction * glm::length(friction * normalForce);

    // if result force in direction of veloctiy is less than full friction
    // force, than rigid bodu will stop.
    auto projectionVel = Projection(velocity, direction);
    auto projectionForce = Projection(m_ResForce, direction);
    if (glm::length(projectionForce +
            + (projectionVel / (dt * massInverse)))
            < glm::length(frictionForce)) {
        m_ResForce -= m_ResForce * glm::abs(direction);
        velocity -= velocity * glm::abs(direction);
        LimitTorque(frictionForce, r);
        return;
    }

    // otherwise apply full friction force
    m_ResForce += frictionForce;
    LimitTorque(frictionForce, r);
}

inline float getRestitution(RigidBody *rigidBody, RigidBody *otherRigidBody) {
    float e = std::min(rigidBody->restitution, otherRigidBody->restitution);
    if (rigidBody->massInverse == 0 || otherRigidBody->massInverse == 0) {
        return -e;
    }
    return -(1 + e);
}

inline Vec3 ImpulseForce(RigidBody *rigidBody, RigidBody *otherRigidBody,
    Vec3 normal, float velAlongNormal, float dt) {
    // Calculate restitution
    auto restitution = getRestitution(rigidBody, otherRigidBody);
    // Calculate impulse scalar
    float impulseScalar = restitution * velAlongNormal;
    impulseScalar /= rigidBody->massInverse + otherRigidBody->massInverse;
    // Apply impulse
    Vec3 impulse = impulseScalar * normal;
    return (impulse / dt);
}

void RigidBody::ComputeForceTorque(RigidBody *otherRigidBody,
        CollisionManifold manifold, Transform transform,
        Transform otherTransform, float dt) {
    if (massInverse == 0 && otherRigidBody->massInverse == 0) {
        return;
    }

    Vec3 rv = velocity - otherRigidBody->velocity;

    Vec3 normal = manifold.normal;
    Logger::Info("normal %s", ToString(normal));

    float velAlongNormal = glm::dot(rv, normal);

    Vec3 impulseForce = ImpulseForce(this, otherRigidBody, normal,
            velAlongNormal, dt);
    Logger::Info("force %s", ToString(m_ResForce));
    Logger::Info("otherforce %s", ToString(otherRigidBody->m_ResForce));
    Logger::Info("impulse force %s", ToString(impulseForce));

    // Compute normal force
    Vec3 normalForce = -Projection(m_ResForce, normal);
    Vec3 otherNormalForce = -Projection(otherRigidBody->m_ResForce, normal);
    m_ResForce += normalForce;
    otherRigidBody->m_ResForce += otherNormalForce;
    Logger::Info("force %s", ToString(m_ResForce));
    Logger::Info("otherforce %s", ToString(otherRigidBody->m_ResForce));


    // If one of body is static (has infinity mass), then other body will stop
    if (massInverse == 0) {
        otherRigidBody->velocity -=
            Projection(otherRigidBody->velocity, normal);
    }
    if (otherRigidBody->massInverse == 0) {
        velocity -= Projection(velocity, -normal);
    }

    // Compute lever of force
    Logger::Info("point %s", ToString(manifold.collisionPoint));
    Logger::Info("c1 %s", ToString(transform.GetTranslation()));
    Logger::Info("c2 %s", ToString(otherTransform.GetTranslation()));
    auto r1 = manifold.collisionPoint - transform.GetTranslation();
    auto r2 = manifold.collisionPoint - otherTransform.GetTranslation();

    // Compute impulse
    if (velAlongNormal < 0) {
        m_ResForce += impulseForce;
        otherRigidBody->m_ResForce -= impulseForce;

        // Compute torque for impulse force
        ApplyTorque(impulseForce, r1);
        Logger::Info("torque %s", ToString(m_Torque));
        otherRigidBody->ApplyTorque(-impulseForce, r2);
        Logger::Info("torque %s", ToString(otherRigidBody->m_Torque));
    }
    Logger::Info("force %s", ToString(m_ResForce));
    Logger::Info("otherforce %s", ToString(otherRigidBody->m_ResForce));

    // Compute friction
    /*
    auto friction = std::sqrt(kineticFriction * otherRigidBody->kineticFriction);
    ComputeFriction(normalForce, friction, r1, dt, normal);
    otherRigidBody->ComputeFriction(otherNormalForce, friction, r2, dt, -normal);
    Logger::Info("force %s", ToString(m_ResForce));
    Logger::Info("otherforce %s", ToString(otherRigidBody->m_ResForce));
    */
}
