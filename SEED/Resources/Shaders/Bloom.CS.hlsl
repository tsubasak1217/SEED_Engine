//============================================================================
//	Bloom.CS
//============================================================================
#include "Object3D.hlsli"

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<Float> threshold : register(b0); // 閾値
ConstantBuffer<Int> radius : register(b1); // ぼかし半径
ConstantBuffer<Float> sigma : register(b2); // ブラー強度

//============================================================================
//	buffer
//============================================================================

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

//============================================================================
//	Function
//============================================================================

// ガウス関数
float Gaussian(float x, float s) {
	
    return exp(-(x * x) / (2.0f * s * s));
}

//============================================================================
//	Main
//============================================================================
[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
	
    uint width, height;
    inputTexture.GetDimensions(width, height);

	// 現在処理中のピクセル
    int2 pixelPos = int2(DTid.xy);

	// 元カラー保持
    float4 sceneColor = inputTexture.Load(int3(pixelPos, 0));
	// 範囲外
    if (pixelPos.x >= width || pixelPos.y >= height) {
        return;
    }

	// サンプリング処理
    float3 bloomAccum = 0.0f;
    float weightSum = 0.0f;
	
    for (int y = -radius.value; y <= radius.value; ++y) {
		
        float wy = Gaussian((float) y, sigma.value);
        for (int x = -radius.value; x <= radius.value; ++x) {
			
            float wx = Gaussian((float) x, sigma.value);
            float w = wx * wy;

            int2 samplePos = pixelPos + int2(x, y);
            samplePos = clamp(samplePos, int2(0, 0), int2(int(width) - 1, int(height) - 1));

            float4 color = inputTexture.Load(int3(samplePos, 0));

			// 輝度抽出
            float luminance = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
            if (luminance < threshold.value) {
                color = float4(0.0f, 0.0f, 0.0f, 0.0f);
            }

            bloomAccum += color.rgb * w;
            weightSum += w;
        }
    }
	
	// 合成処理
    float3 bloomColor = (weightSum > 0.0f) ? bloomAccum / weightSum : 0.0f;
    float3 finalColor = sceneColor.rgb + bloomColor;
    outputTexture[pixelPos] = float4(finalColor, 1.0f);
}