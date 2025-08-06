#pragma once
#include <SEED/Lib/Includes/include_tensors.h>
#include <SEED/Lib/Tensor/Quaternion.h>

struct GPUParticle{
    Vector3 position;
    float rotation;
    Vector3 kScale;
    Vector3 scale;
    Vector3 direction;
    float speed;
    float rotateSpeed;
    float lifeTime;
    float currentTime;
    Vector4 color;
    int32_t textureIdx;
    int32_t particleIndex;
    int32_t alive;// bool
};
