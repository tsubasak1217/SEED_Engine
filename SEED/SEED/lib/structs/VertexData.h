#pragma once
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <stdint.h>

struct VertexData{
    Vector4 position_;
    Vector2 texcoord_;
    Vector3 normal_;
};