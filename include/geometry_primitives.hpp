#pragma once
#include "transform.hpp"
#include "math_types.hpp"
#include <vector>

struct Plane;
struct Triangle;
struct Sphere;
struct Ray;
struct AABB;
struct Line;

struct Interval {
    float min;
    float max;
};

struct AABB {
    Vec3 min, max;

    Interval GetInterval(Vec3 axis);
    Vec3 ClosestPoint(Vec3);
    float Distance2(Vec3);
    AABB PrevState(Vec3, float);

    AABB Transformed(Transform);
};

struct Sphere {
    Vec3 center;
    float radius;

    Vec3 ClosestPoint(Vec3);

    Sphere PrevState(Vec3, float);
    Sphere Transformed(Transform);
};

struct Triangle {
    Vec3 a, b, c;
    Vec3 normal;

    explicit Triangle(Vec3, Vec3, Vec3);

    Interval GetInterval(Vec3 axis);
    Vec3 ClosestPoint(Vec3);
    float Distance2(Vec3);
    Triangle Transformed(Transform);
};

struct Ray {
    Vec3 direction;
    Vec3 origin;

    Ray(Vec3 from, Vec3 to);
};

struct Plane {
    Vec3 normal;
    float d;

    Plane() = default;
    explicit Plane(Triangle t);
    explicit Plane(Vec3 normal, Vec3 point);
    explicit Plane(Vec3 normal, float distance);
};

struct OBB {
    Vec3 center;
    Mat3 axis; // Local x-, y-, and z-axis
    Vec3 halfWidth; // Positive halfwidth extents of OBB along each axis

    Interval GetInterval(Vec3 axisParametr);
    Vec3 ClosestPoint(Vec3);
    float Distance2(Vec3);

    std::vector<Vec3> GetVertices();
    std::vector<Line> GetEdges();
    std::vector<Plane> GetPlanes();

    bool PointIn(Vec3);

    OBB Transformed(Transform);
};

struct Line {
    Vec3 start;
    Vec3 end;
};
