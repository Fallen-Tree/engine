#pragma once
#include <optional>
#include "geometry_primitives.hpp"
#include "model.hpp"

bool CollidePrimitive(AABB, AABB);
bool CollidePrimitive(Sphere, Sphere);
bool CollidePrimitive(Triangle, Triangle);

bool CollidePrimitive(AABB, Triangle);
bool CollidePrimitive(Triangle, AABB);

bool CollidePrimitive(Plane p, AABB a);
bool CollidePrimitive(AABB p, Plane a);

bool CollidePrimitive(AABB, Sphere);
bool CollidePrimitive(Sphere, AABB);

bool CollidePrimitive(Triangle, Sphere);
bool CollidePrimitive(Sphere, Triangle);

template<typename T>
bool CollideModelAt(T t, Model *model, Transform transform);
bool CollideModels(Model *model, Transform transform, Model *model2, Transform transform2);

bool CollidePrimitive(Ray, Sphere);
bool CollidePrimitive(Ray, AABB);

std::optional<float> CollisionPrimitive(Ray, Sphere);
std::optional<float> CollisionPrimitive(Ray, AABB);

Vec3 CollisionNormal(AABB,AABB,Transform,Transform,Vec3,float);
Vec3 CollisionNormal(Sphere,AABB,Transform,Transform,Vec3,float);
Vec3 CollisionNormal(AABB,Sphere,Transform,Transform,Vec3,float);
Vec3 CollisionNormal(AABB,Model*,Transform,Transform,Vec3,float);
Vec3 CollisionNormal(Model*,AABB,Transform,Transform,Vec3,float);
Vec3 CollisionNormal(Sphere,Sphere,Transform,Transform,Vec3,float);
Vec3 CollisionNormal(Sphere,Model*,Transform,Transform,Vec3,float);
Vec3 CollisionNormal(Model*,Sphere,Transform,Transform,Vec3,float);
Vec3 CollisionNormal(Model*,Model*,Transform,Transform,Vec3,float);
