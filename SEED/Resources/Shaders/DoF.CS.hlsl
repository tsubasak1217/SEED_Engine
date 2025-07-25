#include "Object3d.hlsli"

Texture2D<float4> inputTexture : register(t0); // スクショ
RWTexture2D<float4> outputTexture : register(u0); // 出力画像
Texture2D<float4> inputDepthTexture : register(t1); // depthStencilのほう
RWTexture2D<float4> outputDepthTexture : register(u1); // 深度値を画像として書き込む用

ConstantBuffer<Float> resolutionRate : register(b0);
SamplerState gSampler : register(s0);

// ガウシアン重み計算関数
float Gaussian(float x, float sigma) {
    return exp(-0.5 * (x * x) / (sigma * sigma)) / (sqrt(2.0 * 3.141592) * sigma);
}

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    uint2 pixelCoord = DTid.xy;
    if (pixelCoord.x >= 1280 || pixelCoord.y >= 720)
        return;

    float4 currentPixelColor = inputTexture.Load(int3(pixelCoord, 0));
    float depth = inputDepthTexture.Load(int3(pixelCoord, 0)).x;
    depth = DepthToLinear(depth, 0.01f, 1000.0f);
    float focusLevel = CalcFocusLevel(depth);

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
            float sampleFocus = CalcFocusLevel(sampleDepth);

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

    // デバッグ用：フォーカスレベルを可視化
    outputDepthTexture[pixelCoord] = float4(1.0f, 0.0f, 0.0f, 1.0f) * focusLevel;
}