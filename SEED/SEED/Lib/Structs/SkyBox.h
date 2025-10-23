#pragma once
#include <stdint.h>
#include <string>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Structs/Color.h>

/// <summary>
/// スカイボックス(天球のボックス版)
/// </summary>
struct SkyBox{
    static inline uint32_t textureGH_ = 0;
    static inline std::string textureName_ = "";
    static inline Color color_ = {1.0f,1.0f,1.0f,1.0f};
    static inline Vector3 translate_ = { 0.0f,0.0f,0.0f };
    static inline float scale_ = 100.0f;
    static inline bool isFollowToCamera_ = true;
};