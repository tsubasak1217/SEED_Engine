#pragma once
#include "Vector3.h"
#include <stdint.h>

enum LINE_TYPE{
    SEGMENT = 0,
    RAY,
    LINE
};

struct Line{
    Vector3 origin_;
    Vector3 end_;
    int type_;

    Line() = default;
    Line(const Vector3& origin, const Vector3& end, int type = SEGMENT){
        origin_ = origin;
        end_ = end;
        type_ = type;
    }
};