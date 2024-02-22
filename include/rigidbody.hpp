#pragma once

#include "math_types.hpp"
#include "transform.hpp"
#include "collider.hpp"

enum TypeRigidBody{
    LINEAR,
    ANGULAR,
    LINEAR_ANGULAR,
    NONE
};

class RigidBody {
public:
 RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, TypeRigidBody type, 
         float restitution, Vec3 defaultForce, Vec3 defaultTorque);

 void Compute_Force_Torque(float dt);  

 void Update(Transform *tranform, float dt);

 void ResolveCollisions(Transform *tranform, 
        Transform *otherTransform, RigidBody *otherRigidBody, 
        Collider *otherColider, float dt);

 void AddVelocity(Vec3 velocity);

 void SetVelocity(Vec3 velocity);

 void AddInstantForce(Vec3 force);

 void AddDefaultForce(Vec3 force);

 void SetDefaultForce(Vec3 force);

 void AddInstantTorque(Vec3 torque);

 void AddDefaultTorque(Vec3 torque);

 void SetDefaultTorque(Vec3 torque);

 void Mass(float mass);

private:
 void LinearCalculation(Transform *transform, float dt);

 void AngularCalculation(Transform *transform, float dt);

 void CalcForce(float dt);

 void CalcForce(Transform *tranform, Transform *otherTransform, 
         RigidBody *otherRigidBody, Collider *otherColider, float dt);

 void CalcTorque(float dt);

 void CalcTorque(RigidBody *otherRigidBody, float dt);

 float m_Restitution = 1;
 float m_MassInverse;
 Mat3 m_IbodyInverse; // inverse matrix inertia tensor
 Vec3 m_ResForce = Vec3(0); // resultant force
 Vec3 m_DefaultForce = Vec3(0);
 Vec3 m_DefaultTorque = Vec3(0);
 Vec3 m_Torque = Vec3(0);
 Vec3 m_Velocity = Vec3(0);
 TypeRigidBody m_Type;
};

