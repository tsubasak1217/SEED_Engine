#pragma once
#include <algorithm>
#include "Vector3.h"
#include "OBB.h"
#include "AABB.h"
#include "Line.h"
#include "Sphere.h"
#include "Quad.h"
#include "Triangle.h"

struct CollisionData{
    Vector3 pos1;
    Vector3 pos2;

    Vector3 moveVec1;
    Vector3 moveVec2;

    Vector3 preRotate1;
    Vector3 preRotate2;

    Vector3 pushBackAxis;
    float dot = 1.0f;
    float collideDepth = 1000000.0f;
};

namespace Collision{

    namespace OBB{
        bool OBB(const ::OBB& obb1, const ::OBB& obb2);
        bool AABB(const ::OBB& obb, const ::AABB& aabb);
        bool Line(const ::OBB& obb, const ::Line& line);
        bool Sphere(const ::OBB& obb, const ::Sphere& sphere);
    }

    namespace AABB{
        bool OBB(const ::AABB& aabb, const ::OBB& obb);
        bool AABB(const ::AABB& aabb1, const ::AABB& aabb2);
        bool Line(const ::AABB& aabb, const ::Line& line);
        bool Sphere(const ::AABB& aabb, const ::Sphere& sphere);
    }

    namespace Line{
        bool OBB(const ::Line& line, const ::OBB& obb);
        bool AABB(const ::Line& line, const ::AABB& aabb);
        bool Sphere(const ::Line& line, const ::Sphere& sphere);
    }

    namespace Sphere{
        bool OBB(const ::Sphere& sphere, const ::OBB& obb);
        bool AABB(const ::Sphere& sphere, const ::AABB& aabb);
        bool Line(const ::Sphere& sphere, const ::Line& line);
        bool Sphere(const ::Sphere& sphere1, const ::Sphere& sphere2);
    }

    namespace Quad{
        //bool Line(const ::Quad& quad, const ::Line& line);
    }
}

