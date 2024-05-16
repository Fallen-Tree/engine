#pragma once
#include <optional>
#include "geometry_primitives.hpp"
#include "mesh.hpp"
#include "manifold.hpp"

CollisionManifold CollidePrimitive(AABB, AABB);
CollisionManifold CollidePrimitive(Sphere, Sphere);
CollisionManifold CollidePrimitive(Triangle, Triangle);
CollisionManifold CollidePrimitive(OBB, OBB);

CollisionManifold CollidePrimitive(AABB, Triangle);
CollisionManifold CollidePrimitive(Triangle, AABB);

CollisionManifold CollidePrimitive(Plane p, AABB a);
CollisionManifold CollidePrimitive(AABB p, Plane a);

CollisionManifold CollidePrimitive(AABB, Sphere);
CollisionManifold CollidePrimitive(Sphere, AABB);

CollisionManifold CollidePrimitive(Triangle, Sphere);
CollisionManifold CollidePrimitive(Sphere, Triangle);

CollisionManifold CollidePrimitive(Plane, OBB);
CollisionManifold CollidePrimitive(OBB, Plane);

CollisionManifold CollidePrimitive(Sphere, OBB);
CollisionManifold CollidePrimitive(OBB, Sphere);

CollisionManifold CollidePrimitive(Triangle, OBB);
CollisionManifold CollidePrimitive(OBB, Triangle);

CollisionManifold CollidePrimitive(AABB, OBB);
CollisionManifold CollidePrimitive(OBB, AABB);

template<typename T>
CollisionManifold CollideMeshAt(T t, Mesh *mesh, Transform transform);
CollisionManifold CollideMeshes(Mesh *mesh, Transform transform, Mesh *mesh2,
        Transform transform2);

bool CollidePrimitive(Ray, Sphere);
bool CollidePrimitive(Ray, AABB);
bool CollidePrimitive(Ray, OBB);

std::optional<float> CollisionPrimitive(Ray, Sphere);
std::optional<float> CollisionPrimitive(Ray, AABB);
std::optional<float> CollisionPrimitive(Ray, OBB);
