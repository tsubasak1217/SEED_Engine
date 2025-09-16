#pragma once
#include <SEED/Lib/Tensor/Vector2.h>

struct BlurParams{
    Vector2 texelSize; // 1.0f / テクスチャサイズ
    int blurRadius; // ブラー半径
};