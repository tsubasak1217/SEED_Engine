#pragma once
#include <stdint.h>
#include <SEED/Lib/Tensor/Vector4.h>

struct SkyBox{
    static inline uint32_t textureGH_ = 0;
    static inline Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};
};