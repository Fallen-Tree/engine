#pragma once

#include "math_types.hpp"
#include "transform.hpp"
#include "collider.hpp"
#include "collisions.hpp"

//TODO:: make much more IBody getter
Mat3 IBodySphere(float, float);

class RigidBody {
public:
 float restitution;
 float kineticFriction; 
 float massInverse;
 Mat3 ibodyInverse; // inverse matrix inertia tensor
 Vec3 defaultForce;
 Vec3 velocity;
 Vec3 lineraUnlock;
 Vec3 angularUnlock;


 RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, 
         float restitution, Vec3 defaultForce, Vec3 defaultTorque,
         Vec3 lineraUnlock, Vec3 angularUnlock, float kineticFriction); 

 void Update(Transform *tranform, float dt);

 void ResolveCollisions(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt);

void Mass(float mass);

void IbodyInverse(Mat3 iBody);

private:
 void LinearCalculation(Transform *transform, float dt);

 void AngularCalculation(Transform *transform, float dt);

 void ComputeForceTorque(Transform tranform, Transform otherTransform, 
        Collider *collider, Collider *otherCollider, RigidBody *otherRigidBody, 
        float dt);

 void ComputeTorque(Vec3 force, Vec3 r);

 void ComputeFriction(Vec3 normalForce, float friction, Vec3 r);

 Vec3 m_ResForce = Vec3(0); // resultant force 
 Vec3 m_Torque = Vec3(0);
};
