#include "Object3D.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
    int lightingType;
    int GH;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

StructuredBuffer<Material> gMaterial : register(t0, space0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b0);
Texture2D<float4> gTexture[128] : register(t1, space0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    int GH = gMaterial[input.instanceID].GH;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial[input.instanceID].uvTransform);
    float4 textureColor = gTexture[GH].Sample(gSampler, transformedUV.xy);
    
     // 入力が透明の場合は棄却
    if (textureColor.a == 0.0f)
    {
        textureColor.a = 1.0f;
        //discard;
    }
    
    PixelShaderOutput output;
    
    
    if (gMaterial[input.instanceID].lightingType == LightingType::HALF_LAMBERT)// ハーフランバート-------------
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

        output.color = gMaterial[input.instanceID].color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial[input.instanceID].color.a * textureColor.a;
    }
    else if (gMaterial[input.instanceID].lightingType == LightingType::LAMBERT)// ランバート------------------
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        output.color = gMaterial[input.instanceID].color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial[input.instanceID].color.a * textureColor.a;
    }
    else // ライティングなし------------------------------------------------------------------------------------
    {
        output.color = gMaterial[input.instanceID].color * textureColor;
    }
   
    // 出力が透明の場合は棄却
    if (output.color.a == 0.0f)
    {
        output.color.a = 1.0f;
        //discard;
    }
    
    return output;
}
