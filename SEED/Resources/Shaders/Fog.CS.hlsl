#include "Object3d.hlsli"

Texture2D<float4> inputTexture : register(t0); // スクショ
RWTexture2D<float4> outputTexture : register(u0); // 出力画像
Texture2D<float4> inputDepthTexture : register(t1); // depthStencil
SamplerState gSampler : register(s0);
ConstantBuffer<Int> textureWidth : register(b0); // テクスチャの幅
ConstantBuffer<Int> textureHeight : register(b1); // テクスチャの高さ
cbuffer FogParams : register(b2) {
    float4 fogColor; // 霧の色
    float fogStrength; // 霧の強さ(0~1)
    float fogStart;    // 霧の開始深度(0~1)
};

/*- 非線形を線形に変換する関数-*/
float DepthToLinear(float z, float nearPlane, float farPlane) {
    return nearPlane * farPlane / (farPlane - z * (farPlane - nearPlane));
}

// Compute Shaderのメイン関数
[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    uint2 pixelCoord = DTid.xy;
    if (pixelCoord.x >= textureWidth.value || pixelCoord.y >= textureHeight.value)
        return;

    float4 currentPixelColor = inputTexture.Load(int3(pixelCoord, 0));
    float depth = inputDepthTexture.Load(int3(pixelCoord, 0)).x;
    float linearDepth = DepthToLinear(depth, 0.01f, 1000.0f);
    float linearFogStart = DepthToLinear(fogStart, 0.01f, 1000.0f);
    float fogFactor = saturate((depth - fogStart) / (1.0 - fogStart)) * fogStrength;
    float3 finalColor = lerp(currentPixelColor.rgb, fogColor.rgb, fogFactor);
    outputTexture[pixelCoord] = float4(finalColor, currentPixelColor.a);
}