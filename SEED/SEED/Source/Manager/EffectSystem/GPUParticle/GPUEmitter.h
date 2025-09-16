#pragma once
#include <SEED/Lib/Includes/include_tensors.h>

struct GPUEmitter{
    // position
    Vector3 position;
    float pad1;
    Vector3 emitRange;
    float pad2;
    // scale
    Vector3 minScale;
    float pad3;
    Vector3 maxScale;
    float pad4;
    // rotate
    float minRotation;
    float maxRotation;
    float minRotateSpeed;
    float maxRotateSpeed;
    // direction
    Vector3 baseDirection;
    float pad5;
    float angleRange;
    // speed
    float minSpeed;
    float maxSpeed;
    float pad6;
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
    int32_t padding[3]; // 16バイト境界のためのパディング
};