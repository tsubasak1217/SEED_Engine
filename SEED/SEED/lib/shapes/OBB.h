#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <array>

struct OBB{
    Vector3 center;
    Vector3 halfSize = {1.0f,1.0f,1.0f};
    Vector3 rotate;

public:
    std::array<Vector3, 8> GetVertices()const;
};