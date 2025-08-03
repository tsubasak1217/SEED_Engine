#pragma once
#include <SEED/Lib/Includes/include_tensors.h>
#include <SEED/Lib/Tensor/Quaternion.h>

struct GPUParticle{
    Vector3 position;
    Quaternion rotation;
    Quaternion localRotation;
    Vector3 kScale;
    Vector3 scale;
    Vector3 direction;
    float speed;
    Vector3 rotateAxis;
    float rotateSpeed;
    float lifeTime;
    float currentTime;
    Vector4 color;
    int32_t isBillboard;// bool
    int32_t alive;// bool
};
