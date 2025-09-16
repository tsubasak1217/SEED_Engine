// 入力画像（ブラー元）
Texture2D<float4> inputTexture : register(t0);
// 出力画像（中間ぼかし結果）
RWTexture2D<float4> outputTexture : register(u0);
// サンプラ
SamplerState gSampler : register(s0);

cbuffer BlurParams : register(b0) {
    float2 texelSize; // = 1.0 / 画像サイズ（ピクセル -> uv変換用）
    int blurRadius;
}

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    int2 coord = DTid.xy;
    float2 uv = (float2) coord * texelSize;

    float4 result = float4(0, 0, 0, 0);
    float totalWeight = 0.0f;

    for (int i = -blurRadius; i <= blurRadius; ++i) {
        float offset = (float) i;
        float2 sampleUV = uv + float2(0, offset * texelSize.y);
        float weight = exp(-0.5 * (offset * offset) / (blurRadius * blurRadius));
        result += inputTexture.SampleLevel(gSampler, sampleUV, 0) * weight;
        totalWeight += weight;
    }

    result /= totalWeight;
    outputTexture[coord] = result;
}
