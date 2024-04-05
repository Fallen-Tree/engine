#pragma once
#include <optional>
#include "geometry_primitives.hpp"
#include "model.hpp"

bool CollidePrimitive(AABB, AABB);
bool CollidePrimitive(Sphere, Sphere);
bool CollidePrimitive(Triangle, Triangle);
bool CollidePrimitive(OBB, OBB);

bool CollidePrimitive(AABB, Triangle);
bool CollidePrimitive(Triangle, AABB);

bool CollidePrimitive(Plane p, AABB a);
bool CollidePrimitive(AABB p, Plane a);

bool CollidePrimitive(AABB, Sphere);
bool CollidePrimitive(Sphere, AABB);

bool CollidePrimitive(Triangle, Sphere);
bool CollidePrimitive(Sphere, Triangle);

bool CollidePrimitive(Plane, OBB);
bool CollidePrimitive(OBB, Plane);

bool CollidePrimitive(Sphere, OBB);
bool CollidePrimitive(OBB, Sphere);

bool CollidePrimitive(Triangle, OBB);
bool CollidePrimitive(OBB, Triangle);

bool CollidePrimitive(AABB, OBB);
bool CollidePrimitive(OBB, AABB);

template<typename T>
bool CollideModelAt(T t, Model *model, Transform transform);
bool CollideModels(Model *model, Transform transform, Model *model2, Transform transform2);

bool CollidePrimitive(Ray, Sphere);
bool CollidePrimitive(Ray, AABB);
bool CollidePrimitive(Ray, OBB);

std::optional<float> CollisionPrimitive(Ray, Sphere);
std::optional<float> CollisionPrimitive(Ray, AABB);
std::optional<float> CollisionPrimitive(Ray, OBB);
