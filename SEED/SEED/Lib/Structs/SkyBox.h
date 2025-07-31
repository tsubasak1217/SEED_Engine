#pragma once
#include <stdint.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Vector3.h>

struct SkyBox{
    static inline uint32_t textureGH_ = 0;
    static inline Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};
    static inline Vector3 translate_ = { 0.0f,0.0f,0.0f };
    static inline float scale_ = 100.0f;
    static inline bool isFollowCameraPos_ = true;
};