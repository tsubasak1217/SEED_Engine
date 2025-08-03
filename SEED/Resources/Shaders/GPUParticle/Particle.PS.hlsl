#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"

// texture
Texture2D<float4> gTexture[128] : register(t0, space0);
SamplerState gSampler : register(s0);


// Particle�̃s�N�Z���V�F�[�_�[
PixelShaderOutput main(GPUParticleVSOutput input) {

    // �����ȃp�[�e�B�N���͊��p
    if (input.particleTextureIdx == -1) {
        discard;
    }

    //int GH = gMaterial[input.instanceID].GH;
    float4 textureColor = gTexture[input.particleTextureIdx].Sample(gSampler, input.texcoord);
    
     // ���͂������̏ꍇ�͊��p
    if (textureColor.a <= 1e-5f) {
        discard;
    }
    
    PixelShaderOutput output;
    output.color = input.particleColor * textureColor;

    
    // �o�͂������̏ꍇ�͊��p
    if (output.color.a <= 1e-5f) {
        discard;
    }
    
    return output;
}
