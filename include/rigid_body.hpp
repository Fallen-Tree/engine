#pragma once

#include "math_types.hpp"
#include "transform.hpp"
#include "collider.hpp"
#include "collisions.hpp"

//TODO:: make much more IBody getter
Mat3 IBodySphere(float radius, float mass);
Mat3 IBodyOBB(Vec3 halfWidth, float mass);

class RigidBody {
public:
 // degree of elasticity 
 // should be in [0, 1]
 // the more, the stronger elasticity
 float restitution;
 float kineticFriction; 
 float massInverse;
 // inverse matrix inertia tensor
 Mat3 ibodyInverse;
 Vec3 defaultForce;
 Vec3 velocity = Vec3(0);
 // angilarUnlock is unlock for every angulat axis
 // should be in {0, 1}
 Vec3 angularUnlock = Vec3(1);


 RigidBody() = default;
 RigidBody(float mass, Mat3 iBody, Vec3 initalVelocity, 
         float restitution, Vec3 defaultForce,
         Vec3 angularUnlock, float kineticFriction); 

 RigidBody(float mass, Mat3 iBody, float restitution, Vec3 defaultForce,
         float kineticFriction); 

 void Update(Transform *tranform, float dt);

 void ResolveCollisions(RigidBody *otherRigidBody, CollisionManifold manifold,
         Transform transform, Transform otherTransform, float dt);

 void SetMass(float mass);

 void SetIbodyInverse(Mat3 iBody);

 void ApplyTorque(Vec3 force, Vec3 r);

private:
 void LinearCalculation(Transform *transform, float dt);

 void AngularCalculation(Transform *transform, float dt);

 void ComputeForceTorque(RigidBody* otherRigidBody,
         CollisionManifold manifold, Transform tranform,
         Transform otherTransform, float dt);

 void ComputeFriction(Vec3 normalForce, float friction, Vec3 r, float dt,
         Vec3 normal);

 // approximate m_Torque to torque
 // rate of approximating depends on TORQUE_SMOTHNESS
 void LimitTorque(Vec3 force, Vec3 r);

 // resulant force
 Vec3 m_ResForce = Vec3(0); 
 Vec3 m_Torque = Vec3(0);
};
