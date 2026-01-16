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

namespace SEED{
    /// <summary>
    /// 各形状の当たり判定関数をまとめたnamespace
    /// </summary>
    namespace Collision{

        namespace OBB{
            bool OBB(const Topology::OBB& obb1, const Topology::OBB& obb2);
            bool AABB(const Topology::OBB& obb, const Topology::AABB& aabb);
            bool Line(const Topology::OBB& obb, const Topology::Line& line);
            bool Sphere(const Topology::OBB& obb, const Topology::Sphere& sphere);
            CollisionData Sphere(Collider* obbCollider, Collider* sphereCollider);
        }

        namespace AABB{
            bool OBB(const Topology::AABB& aabb, const Topology::OBB& obb);
            bool AABB(const Topology::AABB& aabb1, const Topology::AABB& aabb2);
            bool Line(const Topology::AABB& aabb, const Topology::Line& line);
            bool Sphere(const Topology::AABB& aabb, const Topology::Sphere& sphere);
            CollisionData Sphere(Collider* aabbCollider, Collider* sphereCollider);
        }

        namespace AABB2D{
            bool AABB2D(const Topology::AABB2D& aabb1, const Topology::AABB2D& aabb2);
            CollisionData2D AABB2D(Collider2D* aabb1, Collider2D* aabb2);
        }

        namespace Line{
            bool OBB(const Topology::Line& line, const Topology::OBB& obb);
            bool AABB(const Topology::Line& line, const Topology::AABB& aabb);
            bool Sphere(const Topology::Line& line, const Topology::Sphere& sphere);
        }

        namespace Sphere{
            bool OBB(const Topology::Sphere& sphere, const Topology::OBB& obb);
            CollisionData OBB(Collider* sphereCollider, Collider* obbCollider);
            bool AABB(const Topology::Sphere& sphere, const Topology::AABB& aabb);
            CollisionData AABB(Collider* sphereCollider, Collider* aabbCollider);
            bool Line(const Topology::Sphere& sphere, const Topology::Line& line);
            bool Sphere(const Topology::Sphere& sphere1, const Topology::Sphere& sphere2);
            CollisionData Sphere(Collider* sphereCollider1, Collider* sphereCollider2);
        }

        namespace Circle{
            bool Circle(const Topology::Circle& circle1, const Topology::Circle& circle2);
            CollisionData2D Circle(Collider2D* circle1, Collider2D* circle2);
            bool Quad2D(const Topology::Circle& circle, const Topology::Quad2D& quad);
            CollisionData2D Quad2D(Collider2D* circle, Collider2D* quad);
        }

        namespace Quad{
            CollisionData Line(const Topology::Quad& quad, const Topology::Line& line);
        }

        namespace Quad2D{
            bool Circle(const Topology::Quad2D& quad, const Topology::Circle& circle);
            CollisionData2D Circle(Collider2D* quad, Collider2D* circle);
            bool Quad2D(const Topology::Quad2D& quad1, const Topology::Quad2D& quad2);
            CollisionData2D Quad2D(Collider2D* quad1, Collider2D* quad2);
        }

        namespace Capsule{
            bool Capsule(const Topology::Capsule& capsule1, const Topology::Capsule& capsule2);
        }

        namespace Capsule2D{
            bool Capsule2D(const Topology::Capsule2D& capsule1, const Topology::Capsule2D& capsule2);
        }
    }
}