#include "rigid_body.hpp"
#include "logger.hpp"
#include "pretty_print.hpp"
#include "engine_config.hpp"

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

RigidBody::RigidBody(float mass, Mat3 iBody, float restitution, Vec3 defaultForce) {
    SetMass(mass);
    SetIbodyInverse(iBody);
    this->restitution = restitution;
    this->defaultForce = defaultForce;
}

RigidBody::RigidBody(float mass, Mat3 iBody, float restitution, Vec3 defaultForce,
         float kineticFriction, TypeFriction typeFriction) {
    SetMass(mass);
    SetIbodyInverse(iBody);
    this->restitution = restitution;
    this->defaultForce = defaultForce;
    this->kineticFriction = kineticFriction;
    this->typeFriction = typeFriction;
}

RigidBody::RigidBody(float mass, Mat3 iBody, Vec3 initalVelocity,
        Vec3 angularInitalVelocity, float restitution, Vec3 defaultForce,
        Vec3 angularUnlock, float kineticFriction, TypeFriction typeFriction) {
    SetMass(mass);
    SetIbodyInverse(iBody);
    this->typeFriction = typeFriction;
    this->velocity = initalVelocity;
    this->restitution = restitution;
    this->defaultForce = defaultForce;
    this->angularUnlock = angularUnlock;
    this->kineticFriction = kineticFriction;
    this->angularVelocity = angularInitalVelocity;
}

void RigidBody::Update(Transform *tranform, float dt) {
    if (massInverse == 0)
        return;

    auto dump = std::pow(DUMP, dt);
    angularVelocity *= dump;

    LinearCalculation(tranform, dt);
    AngularCalculation(tranform, dt);

    m_ResForce = defaultForce;
    m_Torque = Vec3(0);
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

void RigidBody::ResolveCollisions(RigidBody *otherRigidBody,
        CollisionManifold manifold,
        Transform globalTransform, Transform otherGlobalTransform,
        Transform& tr1, Transform& tr2, float dt) {
    if (massInverse == 0 && otherRigidBody->massInverse == 0)
        return;

    if (manifold.penetrationDistance <= EPS)
        manifold.penetrationDistance = 0;

    Vec3 avgNormal = Vec3(0);
    for (int i = 0; i < manifold.pointCnt; i++)
        avgNormal += manifold.normals[i];
    avgNormal /= manifold.pointCnt;

    if (massInverse != 0)
        tr1.Translate(avgNormal * manifold.penetrationDistance * 0.5f);
    if (otherRigidBody->massInverse != 0)
        tr2.Translate(-avgNormal * manifold.penetrationDistance * 0.5f);

    // Compute Force and Torque per collision point/normal
    for (int i = 0; i < manifold.pointCnt; i++) {
        Vec3 rv = velocity - otherRigidBody->velocity;

        Vec3 normal = manifold.normals[i];

        float velAlongNormal = glm::dot(rv, normal);

        if (velAlongNormal > 0)
            return;

        Vec3 impulseForce = ImpulseForce(this, otherRigidBody, normal,
                velAlongNormal, dt);

        // Compute normal force
        Vec3 normalForce = -Projection(m_ResForce, normal);
        Vec3 otherNormalForce = -Projection(otherRigidBody->m_ResForce, normal);
        m_ResForce += normalForce;
        otherRigidBody->m_ResForce += otherNormalForce;

        // If one of body is static (has infinity mass), then other body will stop
        if (massInverse == 0) {
            otherRigidBody->velocity -=
                Projection(otherRigidBody->velocity, normal);
        }
        if (otherRigidBody->massInverse == 0) {
            velocity -= Projection(velocity, -normal);
        }

        auto collisionPoint = manifold.points[i];

        // Compute lever of force
        auto r1 = collisionPoint - globalTransform.GetTranslation();
        auto r2 = collisionPoint - otherGlobalTransform.GetTranslation();

        ApplyTorque(normalForce, r1);
        otherRigidBody->ApplyTorque(otherNormalForce, r2);

        // Compute impulse
        if (velAlongNormal <= 0) {
            m_ResForce += impulseForce;
            otherRigidBody->m_ResForce -= impulseForce;

            // Compute torque for impulse force
            ApplyTorque(impulseForce, r1);
            otherRigidBody->ApplyTorque(-impulseForce, r2);
        }

        // Compute friction
        auto friction = std::sqrt(kineticFriction * otherRigidBody->kineticFriction);
        ComputeFriction(
                normalForce, friction, r1, dt, normal, globalTransform);
        otherRigidBody->ComputeFriction(
                otherNormalForce, friction, r2, dt, -normal, otherGlobalTransform);
    }
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
    Vec3 omega = Vec3(0);
    if (m_Torque != Vec3(0)) {
        Mat3 rotation = transform->GetRotation();
        Mat3 Iinverse = rotation * ibodyInverse * glm::transpose(rotation);
        Vec3 L = m_Torque * dt;
        omega = Iinverse * L;
    }
    angularVelocity += omega;
    auto angle = angularVelocity * angularUnlock;
    Mat3 r = transform->GetRotation();
    Mat3 mat = (Mat3(
        glm::cross(angle, r[0]),
        glm::cross(angle, r[1]),
        glm::cross(angle, r[2])));
    transform->SetRotation(Mat4(glm::normalize((glm::quat(r + mat * dt)))));
}

void RigidBody::ApplyTorque(Vec3 force, Vec3 r) {
    m_Torque += glm::cross(r, force) * static_cast<float>(TORQUE_RATIO);
}

inline Vec3 CalculateFrictionDirection(Vec3 normal, Vec3 velocity) {
    Vec3 res = Norm(velocity - Norm(normal) * glm::dot(velocity, Norm(normal)));
    return res;
}

inline Vec3 RigidBody::ComputeSlidingFriction(
        Vec3 normalForce,
        float friction,
        Vec3 direction) {
    return -direction * glm::length(friction * normalForce);
}

inline Vec3 RigidBody::ComputeRollingFriction(
        Vec3 normalForce,
        float friction,
        Vec3 direction,
        Transform transform) {
    return -direction * glm::length(
            normalForce * (friction / transform.GetScale().x));
}

inline void RigidBody::ComputeFriction(
        Vec3 normalForce,
        float friction,
        Vec3 r,
        float dt,
        Vec3 normal,
        Transform transform) {
    if (massInverse == 0)
        return;

    Vec3 direction = CalculateFrictionDirection(normal, velocity);
    Vec3 frictionForce = Vec3(0);
    switch (typeFriction) {
        case slidingFriction:
            frictionForce = ComputeSlidingFriction(
                    normalForce, friction, direction);
            break;
        case rollingFriction:
            frictionForce = ComputeRollingFriction(
                    normalForce, friction, direction, transform);
            ApplyTorque(normalForce, -direction * friction * glm::length(velocity));
            break;
        case emptyFriction:
            return;
        default:
            Logger::Error("Unknown type of friction");
            break;
    }

    // if result force in direction of veloctiy is less than full friction
    // force, than rigid bodu will stop.
    auto projectionVel = Projection(velocity, direction);
    auto projectionForce = Projection(m_ResForce, direction);
    if (glm::length(projectionForce + (projectionVel / (dt * massInverse)))
            < glm::length(frictionForce)) {
        frictionForce = -Projection(m_ResForce, direction);
    }

    m_ResForce += frictionForce;
    ApplyTorque(frictionForce, r);
}
