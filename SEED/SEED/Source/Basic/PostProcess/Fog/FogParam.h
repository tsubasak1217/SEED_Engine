#pragma once
#include <SEED/Lib/Tensor/Vector4.h>

struct FogParams{
    Vector4 fogColor = Vector4(0.7f, 0.8f, 1.0f, 1.0f); // フォグの色
    float fogStrength = 0.5f; // フォグの強さ
    float fogStartDistance = 10.0f; // フォグが開始する距離
};