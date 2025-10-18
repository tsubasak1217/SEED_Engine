#pragma once
#include <cstdint>

// パーティクルの発生方法
enum class EmitType : int32_t{
    kOnce = 0,// 一度だけ発生
    kInfinite,// 無限に発生
    kCustom,// カスタム
};

enum class EmitterGroupType : int32_t{
    kEmitterGroup3D = 0,
    kEmitterGroup2D
};