#include "Object3d.hlsli"
#include "Material.hlsli"

// material
StructuredBuffer<Material> gMaterial : register(t0, space0);

// texture
TextureCube<float4> gTexture[128] : register(t1, space0);
SamplerState gSampler : register(s0);


// SkyBoxのピクセルシェーダー
PixelShaderOutput main(SkyBoxVSOutput input) {
    int GH = gMaterial[input.instanceID].GH;
    float4 textureColor = gTexture[GH].Sample(gSampler, input.texcoord);
    
     // 入力が透明の場合は棄却
    if (textureColor.a <= 1e-5f) {
        discard;
    }
    
    PixelShaderOutput output;
    output.color = gMaterial[input.instanceID].color * textureColor;

    
    // 出力が透明の場合は棄却
    if (output.color.a <= 1e-5f) {
        discard;
    }
    
    return output;
}
