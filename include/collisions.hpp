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


Vec3 CollisionNormal(AABB, AABB, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(Sphere, AABB, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(AABB, Sphere, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(AABB, Mesh*, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(Mesh*, AABB, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(Sphere, Sphere, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(Sphere, Mesh*, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(Mesh*, Sphere, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(Mesh*, Mesh*, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(OBB, OBB, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(AABB, OBB, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(OBB, AABB, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(Sphere, OBB, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(OBB, Sphere, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(OBB, Mesh*, Transform, Transform, Vec3, float);
Vec3 CollisionNormal(Mesh*, OBB, Transform, Transform, Vec3, float);
