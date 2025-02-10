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
#include <SEED/Source/Object/Collision/Collider.h>
#include <SEED/Source/Manager/CollisionManager/CollisionData.h>

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
        CollisionData Sphere(Collider* sphereCollider1, Collider* sphereCollider2);
    }

    namespace Quad{
        CollisionData Line(const ::Quad& quad, const ::Line& line);
    }

    namespace Capsule{
        bool Capsule(const ::Capsule& capsule1, const ::Capsule& capsule2);
    }
}

