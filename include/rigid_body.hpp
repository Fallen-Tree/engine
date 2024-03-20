#pragma once

#include "math_types.hpp"
#include "transform.hpp"
#include "collider.hpp"
#include "collisions.hpp"

//TODO:: make much more IBody getter
Mat3 IBodySphere(float radius, float mass);

class RigidBody {
public:
// 
 float restitution;
 float kineticFriction; 
 float massInverse;
 Mat3 ibodyInverse; // inverse matrix inertia tensor
 Vec3 defaultForce;
 Vec3 velocity = Vec3(0);
 // linearUnlock is unlock for x, y, z linear axis
 // should have value = {0, 1}
 Vec3 linearUnlock = Vec3(1);
 // angilarUnlock is unlock for every angulat axis
 // should have value = {0, 1}
 Vec3 angularUnlock = Vec3(1);


 RigidBody() = default;
 RigidBody(float mass, Mat3 iBody, Vec3 initalVelocity, 
         float restitution, Vec3 defaultForce, Vec3 lineraUnlock,
         Vec3 angularUnlock, float kineticFriction); 

 RigidBody(float mass, Mat3 iBody, float restitution, Vec3 defaultForce,
         float kineticFriction); 

 void Update(Transform *tranform, float dt);

 void ResolveCollisions(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt);

void SetMass(float mass);

void SetIbodyInverse(Mat3 iBody);

void ApplyTorque(Vec3 force, Vec3 r);

void LimitTorque(Vec3 force, Vec3 r);

private:
 void LinearCalculation(Transform *transform, float dt);

 void AngularCalculation(Transform *transform, float dt);

 void ComputeForceTorque(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt);

 void ComputeFriction(Vec3 normalForce, float friction, Vec3 r, float dt);

 Vec3 m_ResForce = Vec3(0); // resultant force 
 Vec3 m_Torque = Vec3(0);
};
