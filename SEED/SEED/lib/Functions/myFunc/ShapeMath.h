#pragma once
#include "include_shapes.h"
#include "Vector3.h"
#include "Vector2.h"
#include <stdint.h>
#include <cmath>
#include <numbers>

//---------------------- 正三角形 -------------------------

Triangle MakeEqualTriangle(float radius, const Vector4& color);
Triangle2D MakeEqualTriangle2D(float radius, const Vector4& color);

//---------------------- 正四角形 -------------------------

Quad MakeEqualQuad(float radius, const Vector4& color);
Quad2D MakeEqualQuad2D(float radius, const Vector4& color);