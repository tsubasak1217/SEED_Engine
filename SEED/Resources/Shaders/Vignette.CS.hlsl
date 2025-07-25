#include "Object3d.hlsli"
Texture2D<float4> inputTexture : register(t0); // 入力画像
RWTexture2D<float4> outputTexture : register(u0); // 出力画像
ConstantBuffer<Float> vignetteStrength : register(b0); // ビネット効果の強さ
SamplerState gSampler : register(s0); // サンプラ

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    uint2 pixelCoord = DTid.xy;

    if (pixelCoord.x >= 1280 || pixelCoord.y >= 720) {
        return;
    }

    // uv座標の計算（0〜1）
    float2 uv = float2(pixelCoord) / float2(1280.0, 720.0);

    // テクスチャから直接Load（整数座標！）
    float4 color = inputTexture.Load(int3(pixelCoord, 0));

    // ビネット効果
    float2 center = float2(0.5, 0.5);
    float dist = distance(uv, center);

    float vignette = (1.0 - vignetteStrength.value) + saturate(1.0 - dist * dist * 2.5) * vignetteStrength.value;

    color.rgb *= vignette;

    outputTexture[pixelCoord] = color;
}