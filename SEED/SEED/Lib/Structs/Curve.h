#pragma once
#include <vector>
#include <SEED/Lib/Includes/include_tensors.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>
#include <json.hpp>

using Points = std::vector<Vector2>;

// カーブ構造体
struct Curve{
    std::vector<Points> curves;
    Easing::Type curveType = Easing::Type::InOutSine;
    float GetValue(float t, int channel = 0) const;
    void FromJson(const nlohmann::json& json);
};