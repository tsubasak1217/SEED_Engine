//============================================================================
//	Glitch.CS
//============================================================================
#include "Object3D.hlsli"

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<Float> time : register(b0);
ConstantBuffer<Float> intensity : register(b1);
ConstantBuffer<Float> blockSize : register(b2);
ConstantBuffer<Float> colorOffset : register(b3);
ConstantBuffer<Float> noiseStrength : register(b4);
ConstantBuffer<Float> lineSpeed : register(b5);

//============================================================================
//	Texture
//============================================================================

RWTexture2D<float4> outputTexture : register(u0);
Texture2D<float4> inputTexture : register(t0);
Texture2D<float> noiseTexture : register(t1);

SamplerState gSampler : register(s0);

//============================================================================
//	Functions
//============================================================================

float Hash12(float2 p) {
	
    float3 p3 = frac(float3(p.xyx) * 0.1031f);
    p3 += dot(p3, p3.yzx + 33.33f);
    return frac((p3.x + p3.y) * p3.z);
}

//============================================================================
//	main
//============================================================================
[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
	
    uint width, height;
    inputTexture.GetDimensions(width, height);
	
	// 範囲外アクセス回避
    if (DTid.x >= width || DTid.y >= height) {
        return;
    }
	
    float2 uv = (float2) DTid.xy / float2(width, height);
	
	// 横スキャンラインずらし
    float linePhase = Hash12(float2(0.0f, DTid.y * 7.13f) + time.value * lineSpeed.value);
	// たまに大きく跳ねるブロックグリッチ
    float lineAmp = smoothstep(0.8f, 1.0f, linePhase);
	// ブロック単位で段差状にしたい場合
    float blockStep = round(DTid.x / blockSize.value) * blockSize.value;
    float2 glitchOffs = float2(lineAmp * intensity.value * blockSize.value, 0);

	// 元ピクセルとずらしたピクセル取得
    int2 srcPos = int2(clamp(int(DTid.x + glitchOffs.x), 0, int(width - 1)), DTid.y);
    float4 srcColor = inputTexture.Load(int3(srcPos, 0));
	
	// Chromatic Aberration風
    float2 rgbShift = float2((Hash12(float2(time.value, srcPos.y)) - 0.5f) * 2.0f,
	(Hash12(float2(time.value + 11.3f, srcPos.y)) - 0.5f) * 2.0f) * colorOffset.value * intensity.value;

	// r g,bそれぞれ別座標からサンプル
    int2 offR = clamp(srcPos + int2(rgbShift), int2(0, 0), int2(width - 1, height - 1));
    int2 offB = clamp(srcPos + int2(-rgbShift), int2(0, 0), int2(width - 1, height - 1));

    float3 color;
    color.r = inputTexture.Load(int3(offR, 0)).r;
    color.g = srcColor.g;
    color.b = inputTexture.Load(int3(offB, 0)).b;

	// ホワイトノイズオーバーレイ
    float noise = noiseTexture.SampleLevel(gSampler, uv * 512.0f, 0.0f).r;
    color = lerp(color, float3(noise, noise, noise), noiseStrength.value * intensity.value);

	// 最終的な色を出力
    outputTexture[DTid.xy] = float4(color, 1.0f);
}