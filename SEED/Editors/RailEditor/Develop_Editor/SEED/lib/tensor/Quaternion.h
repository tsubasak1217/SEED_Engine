#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

struct Quaternion{
    
public:

    float x;
    float y;
    float z;
    float w;

public:

    Matrix4x4 MakeMatrix()const;
    Quaternion AngleAxis(float angle, const Vector3& axis);
};