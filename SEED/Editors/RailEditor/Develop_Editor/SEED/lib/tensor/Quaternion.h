#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

struct Quaternion{
    
public:

    Vector3 axis;
    float angle;

public:

    Matrix4x4 MakeMatrix()const;

};