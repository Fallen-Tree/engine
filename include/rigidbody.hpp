#pragma once

#include "math_types.hpp"
#include "transform.hpp"
#include "collider.hpp"
#include "collisions.hpp"

//TODO:: make much more IBody getter
Mat3 IBodySphere(float, float);

class RigidBody {
public:
 float restitution = 1;
 float massInverse;
 Mat3 ibodyInverse; // inverse matrix inertia tensor
 Vec3 defaultForce;
 Vec3 defaultTorque;
 Vec3 velocity;
 Vec3 lineraUnlock;
 Vec3 angularUnlock;


 RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, 
         float restitution, Vec3 defaultForce, Vec3 defaultTorque,
         Vec3 lineraUnlock, Vec3 angularUnlock);

 void Update(Transform *tranform, float dt);

 void ResolveCollisions(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt);

void Mass(float mass);

void IbodyInverse(Mat3 iBody);

private:
 void LinearCalculation(Transform *transform, float dt);

 void AngularCalculation(Transform *transform, float dt);

 void Compute(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt);
 void CalcTorque(Vec3 force, Vec3 r);

 Vec3 m_ResForce = Vec3(0); // resultant force 
 Vec3 m_Torque = Vec3(0); // resulant torque
};

