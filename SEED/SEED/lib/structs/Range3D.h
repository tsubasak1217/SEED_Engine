#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <algorithm>

/// <summary>
/// 3次元の範囲を表す構造体
/// </summary>
struct Range3D{

    Range3D() = default;
    Range3D(const Vector3& min, const Vector3& max){
        this->min = Vector3((std::min)(min.x, max.x), (std::min)(min.y, max.y),(std::min)(min.z,max.z));
        this->max = Vector3((std::max)(min.x, max.x), (std::max)(min.y, max.y),(std::max)(min.z,max.z));
    }

    Vector3 min;
    Vector3 max;
};

// Range3DをJSONに変換する関数
inline void to_json(nlohmann::json& j, const Range3D& range){
    j["min"] = range.min;
    j["max"] = range.max;
}


// JSON から Range3D に変換
inline void from_json(const nlohmann::json& j, Range3D& range){
    range.min = j.value("min", 0.0f);
    range.min = j.value("max", 0.0f);
}