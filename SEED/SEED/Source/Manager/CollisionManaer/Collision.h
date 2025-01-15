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
#include "CollisionManaer/CollisionData.h"

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
        CollisionData Line(const ::Quad& quad, const ::Line& line);
    }

    namespace Capsule{
        bool Capsule(const ::Capsule& capsule1, const ::Capsule& capsule2);
    }
}

