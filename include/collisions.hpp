#pragma once
#include "math_types.hpp"

bool Collide(AABB, Plane);
bool Collide(Plane, AABB);

bool Collide(AABB, AABB);

bool Collide(AABB, Triangle);
bool Collide(Triangle, AABB);

bool Collide(Sphere, Sphere);

bool Collide(Sphere, AABB);
bool Collide(AABB, Sphere);

bool Collide(Sphere, Triangle);
bool Collide(Triangle, Sphere);

bool Collide(Triangle, Triangle);

