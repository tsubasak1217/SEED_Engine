#pragma once
#include "Vector3.h"
#include "MatrixFunc.h"
#include <array>

struct OBB{
    Vector3 center;
    Vector3 halfSize;
    Vector3 rotate;

public:
    std::array<Vector3, 8> GetVertex()const;
};