#pragma once

#include "math_types.hpp"
#include "transform.hpp"
#include "collider.hpp"
#include "collisions.hpp"

// TODO(solloballon): make much more IBody getter
Mat3 IBodySphere(float radius, float mass);
Mat3 IBodyOBB(Vec3 halfWidth, float mass);

enum TypeFriction {
    slidingFriction,
    rollingFriction,
    emptyFriction
};

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
 Vec3 angularVelocity = Vec3(0); 
 // angilarUnlock is unlock for every angulat axis
 // should be in {0, 1}
 Vec3 angularUnlock = Vec3(1);

 TypeFriction typeFriction = TypeFriction::emptyFriction;

 RigidBody() = default;
 RigidBody(float mass,
         Mat3 iBody,
         Vec3 initalVelocity,
         Vec3 initalAngVelocity, 
         float restitution,
         Vec3 defaultForce,
         Vec3 angularUnlock,
         float kineticFriction,
         TypeFriction typeFriction); 

 RigidBody(float mass,
         Mat3 iBody,
         float restitution,
         Vec3 defaultForce,
         float kineticFriction,
         TypeFriction typeFriction); 

 RigidBody(float mass,
         Mat3 iBody,
         float restitution,
         Vec3 defaultForce); 

 void Update(Transform *tranform, float dt);

 void ResolveCollisions(RigidBody *otherRigidBody, CollisionManifold manifold,
         Transform globalTransform, Transform globalOtherTransform,
         Transform& tr1, Transform& tr2, float dt);

 void SetMass(float mass);

 void SetIbodyInverse(Mat3 iBody);

 void ApplyTorque(Vec3 force, Vec3 r);

private:
 void LinearCalculation(Transform *transform, float dt);

 void AngularCalculation(Transform *transform, float dt);

 void ComputeFriction(Vec3 normalForce, float friction, Vec3 r, float dt,
         Vec3 normal, Transform tranform);

 Vec3 ComputeRollingFriction(Vec3 normalForce, float friction, Vec3 direction,
         Transform transform);

 Vec3 ComputeSlidingFriction(Vec3 normalForce, float friction, Vec3 direction);

 // resulant force
 Vec3 m_ResForce = Vec3(0); 
 Vec3 m_Torque = Vec3(0);
};

