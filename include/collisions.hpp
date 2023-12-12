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
