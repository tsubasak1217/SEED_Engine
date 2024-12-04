#include "Object3D.hlsli"

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

Texture2D<float4> inputDepthTexture : register(t1);// depthStencilのほう
RWTexture2D<float4> outputDepthTexture : register(u1);// depthTextureのほう

SamplerState gSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    float resolutionRate;
}

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    // ピクセル座標の取得
    uint2 pixelCoord = DTid.xy;
    
    if (pixelCoord.x >= 1280 || pixelCoord.y >= 720)
    {
        return;
    }
    
    // 色の取得
    float4 currentPixelColor = inputTexture.Load(int3(pixelCoord, 0));
    int radius = int(ceil(8.0f * resolutionRate));
    uint2 currentPixelCoord = uint2(0, 0);
    float pixelCount = 0;
    float4 blurredColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 outputColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    

    // テクスチャから深度データを取得
    float depthData = inputDepthTexture.Load(int3(DTid.xy, 0)).x;
    depthData = DepthToLinear(depthData, 0.01f, 1000.0f);
    float focusLevel = CalcDepthData(depthData);
    
    
    // ============================= 大ボケ画像の作成 ================================ //
    // BOX型に一定範囲の色を足す
    for (int row = -radius; row < radius; row++)
    {
        currentPixelCoord.x = pixelCoord.x + row;
        currentPixelCoord.x = clamp(currentPixelCoord.x, 0, 1279);
        
        for (int col = -radius; col < radius; col++)
        {
            currentPixelCoord.y = pixelCoord.y + col;
            currentPixelCoord.y = clamp(currentPixelCoord.y, 0, 719);
            
            // 今のピクセルの深度情報を参照して、ボケていない部分はあまり色に影響を与えないようにする
            float currentDepth = inputDepthTexture.Load(int3(currentPixelCoord, 0)).x;
            currentDepth = DepthToLinear(currentDepth, 0.1f, 1000.0f);
            //currentDepth = CalcDepthData(currentDepth);
            
            
            float blurLevel = 1.0f - currentDepth;
            
            blurredColor += inputTexture.Load(int3(currentPixelCoord, 0)) * blurLevel;
            pixelCount += blurLevel;
        }
    }
    
    // 数で割る
    blurredColor /= pixelCount;
    
    // ============================== 深度情報と照らし合わせて最終的な色を決定 ===========================//
    
    
    outputColor = (focusLevel * currentPixelColor) + ((1.0f - focusLevel) * blurredColor);
    
    // fog
    //float4 fogColor = (blurredColor * 0.7f) + (float4(0.3f, 0.3f, 0.3f, 0.3f) * focusLevel);
    //outputColor = (focusLevel * currentPixelColor) + ((1.0f - focusLevel) * fogColor);
        

    // 加工
    outputTexture[pixelCoord] = blurredColor;
    
    // 深度情報の計算
    outputDepthTexture[pixelCoord] = float4(1.0f, 1.0f, 1.0f, 1.0f) * depthData;

}