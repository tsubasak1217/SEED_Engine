#pragma once
#include <SEED/Lib/Tensor/Vector2.h>
#include <algorithm>

/// <summary>
/// 2次元の範囲を表す構造体
/// </summary>
struct Range2D{

    Range2D() = default;
    Range2D(const Vector2& min, const Vector2& max){
        this->min = Vector2((std::min)(min.x,max.x), (std::min)(min.y,max.y));
        this->max = Vector2((std::max)(min.x,max.x), (std::max)(min.y,max.y));
    }

    Vector2 min;
    Vector2 max;
};

// Range2DをJSONに変換する関数
inline void to_json(nlohmann::json& j, const Range2D& range){
    j["min"] = range.min;
    j["max"] = range.max;
}


// JSON から Range1D に変換
inline void from_json(const nlohmann::json& j, Range2D& range){
    range.min = j.value("min", 0.0f);
    range.min = j.value("max", 0.0f);
}