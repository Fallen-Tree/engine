#pragma once
#include <optional>
#include "geometry_primitives.hpp"
#include "mesh.hpp"
#include "manifold.hpp"

void CollidePrimitive(AABB, AABB, CollisionManifold*);
void CollidePrimitive(Sphere, Sphere, CollisionManifold*);
void CollidePrimitive(Triangle, Triangle, CollisionManifold*);
void CollidePrimitive(OBB, OBB, CollisionManifold*);

void CollidePrimitive(AABB, Triangle, CollisionManifold*);
void CollidePrimitive(Triangle, AABB, CollisionManifold*);

void CollidePrimitive(Plane p, AABB a, CollisionManifold*);
void CollidePrimitive(AABB p, Plane a, CollisionManifold*);

void CollidePrimitive(AABB, Sphere, CollisionManifold*);
void CollidePrimitive(Sphere, AABB, CollisionManifold*);

void CollidePrimitive(Triangle, Sphere, CollisionManifold*);
void CollidePrimitive(Sphere, Triangle, CollisionManifold*);

void CollidePrimitive(Plane, OBB, CollisionManifold*);
void CollidePrimitive(OBB, Plane, CollisionManifold*);

void CollidePrimitive(Sphere, OBB, CollisionManifold*);
void CollidePrimitive(OBB, Sphere, CollisionManifold*);

void CollidePrimitive(Triangle, OBB, CollisionManifold*);
void CollidePrimitive(OBB, Triangle, CollisionManifold*);

void CollidePrimitive(AABB, OBB, CollisionManifold*);
void CollidePrimitive(OBB, AABB, CollisionManifold*);

template<typename T>
void CollideMeshAt(T t, Mesh *mesh, Transform transform, CollisionManifold*);
void CollideMeshes(Mesh *mesh, Transform transform, Mesh *mesh2,
        Transform transform2, CollisionManifold*);

bool CollidePrimitive(Ray, Sphere);
bool CollidePrimitive(Ray, AABB);
bool CollidePrimitive(Ray, OBB);

std::optional<float> CollisionPrimitive(Ray, Sphere);
std::optional<float> CollisionPrimitive(Ray, AABB);
std::optional<float> CollisionPrimitive(Ray, OBB);

