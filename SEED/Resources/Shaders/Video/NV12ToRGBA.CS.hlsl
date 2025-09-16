#include "../Object3d.hlsli"
Texture2D<float> texY : register(t0); // R8_UNORM
Texture2D<float2> texUV : register(t1); // R8G8_UNORM
RWTexture2D<float4> texRGBA : register(u0); // RGBA8_UNORM
ConstantBuffer<Int> videoWidth : register(b0); // テクスチャの幅
ConstantBuffer<Int> videoHeight : register(b1); // テクスチャの高さ

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    if (DTid.x >= videoWidth.value || DTid.y >= videoHeight.value)
        return;

    uint2 coord = DTid.xy;

    // サンプリング
    float y = texY.Load(int3(coord, 0)); // 0.0〜1.0
    float2 uv = texUV.Load(int3(coord / 2, 0)); // 0.0〜1.0

    // 0〜1 → 0〜255 → Limited Range補正
    float Y = y * 255.0;
    float U = uv.x * 255.0;
    float V = uv.y * 255.0;

    // スタジオレンジ → フルレンジ正規化
    Y = (Y - 16.0) / 219.0; // Y: 16〜235 → 0〜1
    U = (U - 128.0) / 224.0; // U: 16〜240 → -0.5〜0.5
    V = (V - 128.0) / 224.0;

    // BT.601変換
    float R = saturate(Y + 1.402 * V);
    float G = saturate(Y - 0.344136 * U - 0.714136 * V);
    float B = saturate(Y + 1.772 * U);

    // sRGB補正を行う
    R = pow(R, 2.2);
    G = pow(G, 2.2);
    B = pow(B, 2.2);
    
    texRGBA[coord] = float4(R, G, B, 1.0);
}