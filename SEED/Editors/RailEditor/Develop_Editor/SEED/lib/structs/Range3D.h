#pragma once
#include "Vector3.h"
#include <algorithm>

struct Range3D{

    Range3D() = default;
    Range3D(const Vector3& p1, const Vector3& p2){
        min = Vector3((std::min)(p1.x, p2.x), (std::min)(p1.y, p2.y),(std::min)(p1.z,p2.z));
        max = Vector3((std::max)(p1.x, p2.x), (std::max)(p1.y, p2.y),(std::max)(p1.z,p2.z));
    }

    Vector3 min;
    Vector3 max;
};