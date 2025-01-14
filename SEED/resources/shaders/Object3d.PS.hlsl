#include "Object3D.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
    int lightingType;
    int GH;
    float shinines;
};

struct DirectionalLight
{
    int type;
    float4 color;
    float intensity;
    int isLighting;
    float3 direction;
};

struct Camera {
    float3 position;
};

ConstantBuffer<Camera> gCamera : register(b0);
StructuredBuffer<Material> gMaterial : register(t0, space0);
StructuredBuffer<DirectionalLight> gDirectionalLight : register(t1,space0);
Texture2D<float4> gTexture[128] : register(t2, space0);
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
        discard;
    }
    
    PixelShaderOutput output;
    
    // カメラへのベクトルを求める
    float3 toEye = normalize(gCamera.position - input.worldPosition);
    // 反射ベクトルを求める
    float3 reflectVec = reflect(gDirectionalLight[0].direction, normalize(input.normal));
    // 内積を取る
    float reflectDotEye = dot(reflectVec, toEye);
    // スペキュラーパワーを求める
    float sprcularPow = pow(saturate(reflectDotEye), gMaterial[input.instanceID].shinines);
    // 格納用
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    
    if (gMaterial[input.instanceID].lightingType == LightingType::HALF_LAMBERT)// ハーフランバート-------------
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight[0].direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

        diffuse = gMaterial[input.instanceID].color.rgb * textureColor.rgb * gDirectionalLight[0].color.rgb * cos * gDirectionalLight[0].intensity;
        specular = gMaterial[input.instanceID].color.rgb * gDirectionalLight[0].intensity * sprcularPow * float3(1.0f, 1.0f, 1.0f);
        
        output.color.rgb = diffuse + specular;
        output.color.a = gMaterial[input.instanceID].color.a * textureColor.a;
    }
    else if (gMaterial[input.instanceID].lightingType == LightingType::LAMBERT)// ランバート------------------
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight[0].direction));
        
        diffuse = gMaterial[input.instanceID].color.rgb * textureColor.rgb * gDirectionalLight[0].color.rgb * cos * gDirectionalLight[0].intensity;
        specular = gMaterial[input.instanceID].color.rgb * gDirectionalLight[0].intensity * sprcularPow * float3(1.0f, 1.0f, 1.0f);
        
        output.color.rgb = diffuse + specular;
        output.color.a = gMaterial[input.instanceID].color.a * textureColor.a;
    }
    else // ライティングなし------------------------------------------------------------------------------------
    {
        output.color = gMaterial[input.instanceID].color * textureColor;
    }
   
    // 出力が透明の場合は棄却
    if (output.color.a == 0.0f)
    {
        discard;
    }
    
    return output;
}
