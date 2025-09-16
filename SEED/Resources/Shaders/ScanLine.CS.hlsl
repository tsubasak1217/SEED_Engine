#include "Object3D.hlsli"

Texture2D<float4> inputTexture : register(t0); // 入力画像
RWTexture2D<float4> outputTexture : register(u0); // 出力画像
ConstantBuffer<Float> time : register(b0); // 時間（秒）
ConstantBuffer<Float> stripeFrequency : register(b1); // 縞の頻度
ConstantBuffer<Float> scanLineStrength : register(b2); // 縞の頻度
ConstantBuffer<Int> textureWidth : register(b3); // テクスチャの幅
ConstantBuffer<Int> textureHeight : register(b4); // テクスチャの高さ
SamplerState gSampler : register(s0); // 使用しないけど必須なら残す

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    uint2 pixelCoord = DTid.xy;

    if (pixelCoord.x >= textureWidth.value || pixelCoord.y >= textureHeight.value) {
        return;
    }

    // 入力画像からそのままピクセルカラー取得（Load使用）
    float4 color = inputTexture.Load(int3(pixelCoord, 0));

    // スキャンライン効果の適用
    float scanline = sin(pixelCoord.y * stripeFrequency.value + time.value * 30.0);
    scanline = (1.0f - scanLineStrength.value) + scanLineStrength.value * scanline;

    color.rgb *= scanline;

    outputTexture[pixelCoord] = color;
}