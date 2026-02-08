#pragma once
#include <SEED/Lib/Includes/include_shapes.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Shapes/AABB.h>
#include <SEED/Lib/Shapes/Line.h>
#include <stdint.h>
#include <cmath>
#include <array>
#include <numbers>

namespace SEED{
    namespace Methods{
        namespace Shape{
            //---------------------- 正三角形 -------------------------
            Topology::Triangle MakeEqualTriangle(float radius, const Color& color);
            Topology::Triangle2D MakeEqualTriangle2D(float radius, const Color& color);

            //---------------------- 正四角形 -------------------------

            Topology::Quad MakeEqualQuad(float radius, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
            Topology::Quad2D MakeEqualQuad2D(float radius, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
            Topology::Quad2D MakeBackgroundQuad2D(int32_t layer = 0, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
            Topology::Quad2D MakeFrontQuad2D(int32_t layer = 0, const Color& color = { 1.0f,1.0f,1.0f,1.0f });

            //---------------------- 矩形 -------------------------
            Topology::Quad MakeQuad(const Vector2& size, const Color& color = { 1.0f,1.0f,1.0f,1.0f }, const Vector2& anchorPoint = { 0.5,0.5f });
            Topology::Quad2D MakeQuad2D(const Vector2& size, const Color& color = { 1.0f,1.0f,1.0f,1.0f }, const Vector2& anchorPoint = { 0.5,0.5f });

            //-------------- 線分同士の最近傍点を求める関数 ---------------
            std::array<Vector3, 2> LineClosestPoints(const Topology::Line& l1, const Topology::Line& l2);
            float LineDistance(const Topology::Line& l1, const Topology::Line& l2);

            //------------------- 最大AABBを求める関数 ------------------

            Topology::AABB MaxAABB(const Topology::AABB& aabb1, const Topology::AABB& aabb2);
            Topology::AABB2D MaxAABB(const Topology::AABB2D& aabb1, const Topology::AABB2D& aabb2);


            //----------------------BOXを作成する関数-------------------

            std::array<Vector3, 8> MakeBox(const Vector3& center, const Vector3& halfSize);
        }
    }
} // namespace SEED