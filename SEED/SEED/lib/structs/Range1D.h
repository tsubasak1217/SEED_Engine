#pragma once
#include <algorithm> // 追加


/// <summary>
/// 1次元の範囲を表す構造体
/// </summary>
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


// Range1DをJSONに変換する関数
inline void to_json(nlohmann::json& j, const Range1D& range){
    j["min"] = range.min;
    j["max"] = range.max;
}


// JSON から Range1D に変換
inline void from_json(const nlohmann::json& j, Range1D& range){
    range.min = j.value("min", 0.0f);
    range.max = j.value("max", 0.0f);
}