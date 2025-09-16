//============================================================================
//	Bloom.CS
//============================================================================
#include "Object3D.hlsli"

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<Float> threshold : register(b0); // 臒l
ConstantBuffer<Int> radius : register(b1); // �ڂ������a
ConstantBuffer<Float> sigma : register(b2); // �u���[���x

//============================================================================
//	buffer
//============================================================================

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

//============================================================================
//	Function
//============================================================================

// �K�E�X�֐�
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

	// ���ݏ������̃s�N�Z��
    int2 pixelPos = int2(DTid.xy);

	// ���J���[�ێ�
    float4 sceneColor = inputTexture.Load(int3(pixelPos, 0));
	// �͈͊O
    if (pixelPos.x >= width || pixelPos.y >= height) {
        return;
    }

	// �T���v�����O����
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

			// �P�x���o
            float luminance = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
            if (luminance < threshold.value) {
                color = float4(0.0f, 0.0f, 0.0f, 0.0f);
            }

            bloomAccum += color.rgb * w;
            weightSum += w;
        }
    }
	
	// ��������
    float3 bloomColor = (weightSum > 0.0f) ? bloomAccum / weightSum : 0.0f;
    float3 finalColor = sceneColor.rgb + bloomColor;
    outputTexture[pixelPos] = float4(finalColor, 1.0f);
}