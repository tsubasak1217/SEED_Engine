#pragma once
#include <algorithm>
#include "Vector3.h"
#include "OBB.h"
#include "AABB.h"
#include "Line.h"
#include "Sphere.h"
#include "Quad.h"
#include "Triangle.h"
#include "Capsule.h"
#include "Collision/Collider.h"
#include <optional>

struct CollisionData{
    // 基礎衝突情報
    bool error = false;
    bool isCollide = false;

    // 押し戻しの情報
    std::optional<Vector3> hitNormal;
    std::optional<float> collideDepth = 1000000.0f;
    std::optional<float> pushBackRatio_A;
    std::optional<float> pushBackRatio_B;
    std::optional<float> distance;

    // 使ったり使わなかったりするパラメーター
    std::optional<float> dot = 1.0f;
    std::optional<Vector3> hitPos;
};

namespace Collision{

    namespace OBB{
        bool OBB(const ::OBB& obb1, const ::OBB& obb2);
        bool AABB(const ::OBB& obb, const ::AABB& aabb);
        bool Line(const ::OBB& obb, const ::Line& line);
        bool Sphere(const ::OBB& obb, const ::Sphere& sphere);
        CollisionData Sphere(Collider* obbCollider, Collider* sphereCollider);
    }

    namespace AABB{
        bool OBB(const ::AABB& aabb, const ::OBB& obb);
        bool AABB(const ::AABB& aabb1, const ::AABB& aabb2);
        bool Line(const ::AABB& aabb, const ::Line& line);
        bool Sphere(const ::AABB& aabb, const ::Sphere& sphere);
        CollisionData Sphere(Collider* aabbCollider, Collider* sphereCollider);
    }

    namespace Line{
        bool OBB(const ::Line& line, const ::OBB& obb);
        bool AABB(const ::Line& line, const ::AABB& aabb);
        bool Sphere(const ::Line& line, const ::Sphere& sphere);
    }

    namespace Sphere{
        bool OBB(const ::Sphere& sphere, const ::OBB& obb);
        CollisionData OBB(Collider* sphereCollider, Collider* obbCollider);
        bool AABB(const ::Sphere& sphere, const ::AABB& aabb);
        CollisionData AABB(Collider* sphereCollider, Collider* aabbCollider);
        bool Line(const ::Sphere& sphere, const ::Line& line);
        bool Sphere(const ::Sphere& sphere1, const ::Sphere& sphere2);
    }

    namespace Quad{
        //bool Line(const ::Quad& quad, const ::Line& line);
    }

    namespace Capsule{
        bool Capsule(const ::Capsule& capsule1, const ::Capsule& capsule2);
    }
}

