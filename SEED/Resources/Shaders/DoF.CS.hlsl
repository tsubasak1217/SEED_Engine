#include "Object3d.hlsli"

Texture2D<float4> inputTexture : register(t0); // スクショ
RWTexture2D<float4> outputTexture : register(u0); // 出力画像
Texture2D<float4> inputDepthTexture : register(t1); // depthStencilのほう

ConstantBuffer<Float> resolutionRate : register(b0); // 解像度比率(0.0〜1.0)
ConstantBuffer<Float> focusDepth : register(b1); // ピントが合う中心深度
ConstantBuffer<Float> focusRange : register(b2); // ピントが影響を及ぼす範囲直径(深度の範囲)

SamplerState gSampler : register(s0);

// ガウシアン重み計算関数
float Gaussian(float x, float sigma) {
    return exp(-0.5 * (x * x) / (sigma * sigma)) / (sqrt(2.0 * 3.141592) * sigma);
}

/*- 非線形を線形に変換する関数-*/
float DepthToLinear(float depth, float near, float far) {
    float z = depth * 2.0f - 1.0f;
    float linearDepth = (2.0f * near * far) / (far + near - z * (far - near));
    return (linearDepth - near) / (far - near);
}


// ピントが合う深度と範囲を考慮してフォーカスレベルを計算する関数
float CalcFocusLevel(float _linearDepth, float _focusDepth, float _focusRange) {
    float result;
    float rangeRadius = _focusRange * 0.5f;
    float inverseRange = 1.0f / _focusRange;
    float2 min_max = {
        _focusDepth - rangeRadius,
        _focusDepth + rangeRadius
    };
    
    if (_linearDepth >= min_max.x && _linearDepth <= min_max.y) {
        result = (sin(4.71f + ((_linearDepth - min_max.x) * inverseRange) * 6.28f) + 1.0f) * 0.5f;
    } else {
        result = 0.0f;
    }
    
    return clamp(result, 0.0f, 1.0f);
};


// Compute Shaderのメイン関数
[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    uint2 pixelCoord = DTid.xy;
    if (pixelCoord.x >= 1280 || pixelCoord.y >= 720)
        return;

    float4 currentPixelColor = inputTexture.Load(int3(pixelCoord, 0));
    float depth = inputDepthTexture.Load(int3(pixelCoord, 0)).x;
    depth = DepthToLinear(depth, 0.01f, 1000.0f);
    float focusLevel = CalcFocusLevel(depth,focusDepth.value,focusRange.value);

    int radius = int(ceil(8.0f * resolutionRate.value));
    float sigma = radius / 2.0f;

    float4 blurredColor = float4(0, 0, 0, 0);
    float totalWeight = 0.0f;

    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int2 offset = int2(dx, dy);
            int2 sampleCoord = int2(pixelCoord) + offset;

            // 範囲外アクセス防止
            sampleCoord = clamp(sampleCoord, int2(0, 0), int2(1279, 719));

            float dist = length(float2(dx, dy));
            float weight = Gaussian(dist, sigma);

            float sampleDepth = inputDepthTexture.Load(int3(sampleCoord, 0)).x;
            sampleDepth = DepthToLinear(sampleDepth, 0.01f, 1000.0f);
            float sampleFocus = CalcFocusLevel(sampleDepth, focusDepth.value, focusRange.value);

            // ピクセルごとにボケ度合いに応じて重みを調整（遠くのピクセルはボケる）
            float blurWeight = weight * (1.0f - sampleFocus);
            blurredColor += inputTexture.Load(int3(sampleCoord, 0)) * blurWeight;
            totalWeight += blurWeight;
        }
    }

    blurredColor = totalWeight > 0 ? blurredColor / totalWeight : currentPixelColor;

    // 焦点深度によって元画像とブラー画像をブレンド
    float4 finalColor = lerp(blurredColor, currentPixelColor, focusLevel);
    outputTexture[pixelCoord] = finalColor;
}