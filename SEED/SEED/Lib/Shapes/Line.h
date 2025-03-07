#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <stdint.h>

enum LINE_TYPE{
    SEGMENT = 0,
    RAY,
    LINE
};

struct Line{
    Vector3 origin_;
    Vector3 end_;
    int type_ = SEGMENT;

    Line() = default;
    Line(const Vector3& origin, const Vector3& end, int type = SEGMENT){
        origin_ = origin;
        end_ = end;
        type_ = type;
    }
};