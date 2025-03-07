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

//---------------------- 正三角形 -------------------------

Triangle MakeEqualTriangle(float radius, const Vector4& color);
Triangle2D MakeEqualTriangle2D(float radius, const Vector4& color);

//---------------------- 正四角形 -------------------------

Quad MakeEqualQuad(float radius, const Vector4& color);
Quad2D MakeEqualQuad2D(float radius, const Vector4& color);

//-------------- 線分同士の最近傍点を求める関数 ---------------
std::array<Vector3, 2> LineClosestPoints(const Line& l1, const Line& l2);
float LineDistance(const Line& l1, const Line& l2);

//------------------- 最大AABBを求める関数 ------------------

AABB MaxAABB(const AABB& aabb1, const AABB& aabb2);