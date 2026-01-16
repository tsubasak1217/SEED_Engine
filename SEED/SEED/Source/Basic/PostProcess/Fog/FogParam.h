#pragma once
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Structs/Color.h>

namespace SEED{
    /// <summary>
    /// フォグのパラメーター
    /// </summary>
    struct FogParams{
        Color fogColor = Vector4(0.7f, 0.8f, 1.0f, 1.0f); // フォグの色
        float fogStrength = 0.5f; // フォグの強さ
        float fogStartDistance = 10.0f; // フォグが開始する距離
    };
}