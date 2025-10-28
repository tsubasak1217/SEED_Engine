#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"

// texture
Texture2D<float4> gTexture[256] : register(t0, space0);
SamplerState gSampler : register(s0);


// Particleのピクセルシェーダー
PixelShaderOutput main(GPUParticleVSOutput input) {

    PixelShaderOutput output;
    
    // 無効なパーティクルは棄却
    if (input.particleTextureIdx == -1) {
        output.color = float4(1,1,1,1);
    }

    float4 textureColor = gTexture[input.particleTextureIdx].Sample(gSampler, input.texcoord);
    
     // 入力が透明の場合は棄却
    if (textureColor.a <= 1e-5f) {
        discard;
    }
    
    output.color = input.particleColor * textureColor;

    
    // 出力が透明の場合は棄却
    if (output.color.a <= 1e-5f) {
        discard;
    }
    
    return output;
}
