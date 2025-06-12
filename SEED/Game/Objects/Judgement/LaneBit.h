#pragma once
#include <cstdint>

enum LaneBit{
    LANE_1 = 1 << 0,
    LANE_2 = 1 << 1,
    LANE_3 = 1 << 2,
    LANE_4 = 1 << 3,
    LANE_5 = 1 << 4,
    SIDEFLICK_LEFT = 1 << 5,
    SIDEFLICK_RIGHT = 1 << 6,
    RECTFLICK_LT = 1 << 7,
    RECTFLICK_LB = 1 << 8,
    RECTFLICK_RT = 1 << 9,
    RECTFLICK_RB = 1 << 10,
    WHEEL_UP = 1 << 11,
    WHEEL_DOWN = 1 << 12,
};

const int32_t kLaneBitCount = 13;// レーンのビット数