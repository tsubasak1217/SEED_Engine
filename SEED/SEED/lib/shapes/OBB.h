#pragma once
#include "Vector3.h"
#include "MatrixFunc.h"
#include <array>

struct OBB{
    Vector3 center;
    Vector3 halfSize = {1.0f,1.0f,1.0f};
    Vector3 rotate;

public:
    std::array<Vector3, 8> GetVertices()const;
};