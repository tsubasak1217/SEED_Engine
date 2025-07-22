
Texture2D<float4> inputTexture : register(t0); // スクショ
RWTexture2D<float4> outputTexture : register(u0); // 出力画像
SamplerState gSampler : register(s0);


[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    // ピクセル座標の取得
    uint2 pixelCoord = DTid.xy;
    
    if (pixelCoord.x >= 1280 || pixelCoord.y >= 720) {
        return;
    }
    
    // 色の取得
    float4 currentPixelColor = inputTexture.Load(int3(pixelCoord, 0));

    // grayScaleにして返す
    float grayValue = dot(currentPixelColor.rgb, float3(0.299, 0.587, 0.114));
    outputTexture[DTid.xy] = float4(grayValue, grayValue, grayValue, currentPixelColor.a);
}