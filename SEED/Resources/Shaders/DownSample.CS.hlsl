#include "Object3d.hlsli"

Texture2D<float4> inputTexture : register(t0); // 入力画像
RWTexture2D<float4> outputHalfTexture : register(u0); // 出力画像

ConstantBuffer<Int> textureWidth : register(b0);
ConstantBuffer<Int> textureHeight : register(b1);
ConstantBuffer<Float> resolutionRate : register(b2); // 例: 0.5 なら 1/2スケール
SamplerState gSampler : register(s0);

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    uint2 outputPos = DTid.xy;

    // 出力サイズが resolutionRate に基づいて縮小されている前提
    float2 inputSize = float2(textureWidth.value, textureHeight.value);
    float2 outputSize = inputSize * resolutionRate.value;
    float2 texelScale = inputSize / outputSize;

    // 入力テクスチャに対応する中心位置
    float2 inputCenter = (outputPos + 0.5f) * texelScale;

    // サンプリング半径（範囲は texelScale の大きさに応じて調整）
    int radius = (int) ceil(max(texelScale.x, texelScale.y) * 0.5f);

    float4 sumColor = float4(0, 0, 0, 0);
    float sampleCount = 0.0f;

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            float2 offset = float2(x, y);
            float2 sampleUV = (inputCenter + offset) / inputSize;
            float4 sample = inputTexture.SampleLevel(gSampler, sampleUV, 0);
            sumColor += sample;
            sampleCount += 1.0f;
        }
    }

    outputHalfTexture[outputPos] = sumColor / sampleCount;
}