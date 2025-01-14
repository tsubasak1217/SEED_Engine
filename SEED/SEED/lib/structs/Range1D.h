#pragma once
#include <algorithm> // 追加

struct Range1D{

    Range1D() = default;
    Range1D(float p1, float p2){
        min = (std::min)(p1, p2);
        max = (std::max)(p1, p2);
    }

    float min;
    float max;

public:


    void operator+=(float value){
        min += value;
        max += value;
    }

    void operator-=(float value){
        min -= value;
        max -= value;
    }
};