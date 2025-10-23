#pragma once
#include <algorithm>
#include <optional>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Shapes/OBB.h>
#include <SEED/Lib/Shapes/AABB.h>
#include <SEED/Lib/Shapes/Line.h>
#include <SEED/Lib/Shapes/Sphere.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/Shapes/Triangle.h>
#include <SEED/Lib/Shapes/Capsule.h>
#include <SEED/Lib/Shapes/Circle.h>
#include <SEED/Source/Basic/Collision/3D/Collider.h>
#include <SEED/Source/Basic/Collision/2D/Collider2D.h>
#include <SEED/Source/Manager/CollisionManager/CollisionData.h>

/// <summary>
/// 各形状の当たり判定関数をまとめたnamespace
/// </summary>
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

    namespace AABB2D{
        bool AABB2D(const ::AABB2D& aabb1, const ::AABB2D& aabb2);
        CollisionData2D AABB2D(Collider2D* aabb1, Collider2D* aabb2);
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
        CollisionData Sphere(Collider* sphereCollider1, Collider* sphereCollider2);
    }

    namespace Circle{
        bool Circle(const ::Circle& circle1, const ::Circle& circle2);
        CollisionData2D Circle(Collider2D* circle1, Collider2D* circle2);
        bool Quad2D(const ::Circle& circle, const ::Quad2D& quad);
        CollisionData2D Quad2D(Collider2D* circle, Collider2D* quad);
    }

    namespace Quad{
        CollisionData Line(const ::Quad& quad, const ::Line& line);
    }

    namespace Quad2D{
        bool Circle(const ::Quad2D& quad, const ::Circle& circle);
        CollisionData2D Circle(Collider2D* quad, Collider2D* circle);
        bool Quad2D(const ::Quad2D& quad1, const ::Quad2D& quad2);
        CollisionData2D Quad2D(Collider2D* quad1, Collider2D* quad2);
    }

    namespace Capsule{
        bool Capsule(const ::Capsule& capsule1, const ::Capsule& capsule2);
    }

    namespace Capsule2D{
        bool Capsule2D(const ::Capsule2D& capsule1, const ::Capsule2D& capsule2);
    }
}

