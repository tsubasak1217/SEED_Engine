#pragma once
#include <SEED/Lib/Includes/include_tensors.h>

struct GPUEmitter{
    // position
    Vector3 position;
    Vector3 emitRange;
    // scale
    Vector3 minScale;
    Vector3 maxScale;
    // rotate
    Vector3 minRotation;
    Vector3 maxRotation;
    Vector3 rotateAxis;
    int32_t useRotateAxis;// bool
    float minRotateSpeed;
    float maxRotateSpeed;
    int32_t isBillboard;// bool
    // direction
    Vector3 baseDirection;
    float directionRange;
    // speed
    float minSpeed;
    float maxSpeed;
    // 色
    Vector4 color;
    // parameter
    float minLifeTime;
    float maxLifeTime;
    float interval;
    float currentTime;
    int32_t numEmitEvery;
    int32_t textureIndex;
    int32_t emit;// bool
    int32_t loop;// bool
    int32_t alive;// bool
};