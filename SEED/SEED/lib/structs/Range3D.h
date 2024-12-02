#pragma once
#include "Vector3.h"
#include <algorithm>

struct Range3D{

    Range3D() = default;
    Range3D(const Vector3& min, const Vector3& max){
        this->min = Vector3((std::min)(min.x, max.x), (std::min)(min.y, max.y),(std::min)(min.z,max.z));
        this->max = Vector3((std::max)(min.x, max.x), (std::max)(min.y, max.y),(std::max)(min.z,max.z));
    }

    Vector3 min;
    Vector3 max;
};