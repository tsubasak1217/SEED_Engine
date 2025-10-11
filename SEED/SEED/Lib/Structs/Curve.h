#pragma once
#include <vector>
#include <SEED/Lib/Includes/include_tensors.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>

using Points = std::vector<Vector2>;

// カーブ構造体
struct Curve{
    std::vector<Points> curves;
    Easing::Type curveType = Easing::Type::InOutSine;
};