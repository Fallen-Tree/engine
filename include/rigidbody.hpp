#pragma once

#include "math_types.hpp"
#include "transform.hpp"

enum TypeRigidBody{
    LINEAR,
    ANGULAR,
    LINEAR_ANGULAR,
    NONE
};

class RigidBody {
public:
 RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, 
         TypeRigidBody type, float restitution);
 
 RigidBody(float mass, Mat3 Ibody, Vec3 initalVelocity, TypeRigidBody type, 
         float restitution, Vec3 defaultForce);

 void Compute_Force_Torque(float dt);  

 void Update(Transform *tranform, float dt);

 void ResolveCollisions(RigidBody *otherRigidBody, float dt);

 void AddVelocity(Vec3 velocity);

 void AddAcceleration(Vec3 acceleration);

 void AddInstantaneousForce(Vec3 force);

 void AddDefaultForce(Vec3 force);

 void SetDefaultForce(Vec3 force);

 void Mass(float mass);

private:
 void LinearCalculation(Transform *transform, float dt);

 void AngualarCalculation(Transform *transform, float dt);

 void CalcForce(float dt);

 void CalcForce(RigidBody *otherRigidBody, float dt);

 void CalcTorque(float dt);

 void CalcTorque(RigidBody *otherRigidBody, float dt);

 float m_Restitution = 1;
 float m_MassInverse;
 Mat3 m_IbodyInverse; // inverse matrix inertia tensor
 Vec3 m_ResForce = Vec3(0); // resultant force
 Vec3 m_DefaultForce = Vec3(0);
 Vec3 m_Torque = Vec3(0);
 Vec3 m_Velocity = Vec3(0);
 TypeRigidBody m_Type;
};

