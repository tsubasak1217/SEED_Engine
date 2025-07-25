Texture2D<float4> inputTexture : register(t0); // 入力画像
RWTexture2D<float4> outputTexture : register(u0); // 出力画像
SamplerState gSampler : register(s0); // サンプラ

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    uint2 pixelCoord = DTid.xy;

    if (pixelCoord.x >= 1280 || pixelCoord.y >= 720) {
        return;
    }

    // シフト量（画素単位）
    int2 shift = int2(2, 0);

    // R, G, B 成分をそれぞれずらして取得
    float3 colorR = inputTexture.Load(int3(pixelCoord + shift, 0)).rgb;
    float3 colorG = inputTexture.Load(int3(pixelCoord, 0)).rgb;
    float3 colorB = inputTexture.Load(int3(pixelCoord - shift, 0)).rgb;

    // 各チャンネルだけ抽出して合成
    float3 result;
    result.r = colorR.r;
    result.g = colorG.g;
    result.b = colorB.b;

    outputTexture[pixelCoord] = float4(result, 1.0);
}