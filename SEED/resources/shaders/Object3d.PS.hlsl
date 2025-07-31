#include "Object3d.hlsli"
#include "Lighting.hlsli"
#include "Material.hlsli"
#include "Camera.hlsli"

// camera
StructuredBuffer<Camera> gCamera : register(t0);
ConstantBuffer<Int> gCameraIndex : register(b0);
// material
StructuredBuffer<Material> gMaterial : register(t1, space0);
// light
StructuredBuffer<DirectionalLight> gDirectionalLight : register(t2, space0);
StructuredBuffer<PointLight> gPointLight : register(t3, space0);
StructuredBuffer<SpotLight> gSpotLight : register(t4, space0);
ConstantBuffer<Int> gDirectionalLightCount : register(b1);
ConstantBuffer<Int> gPointLightCount : register(b2);
ConstantBuffer<Int> gSpotLightCount : register(b3);
// texture
TextureCube<float4> gEnvironmentTexture : register(t5, space0);
Texture2D<float4> gTexture[128] : register(t6, space0);
SamplerState gSampler : register(s0);


PixelShaderOutput main(MeshShaderOutput input) {
    int GH = gMaterial[input.instanceID].GH;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial[input.instanceID].uvTransform);
    float4 textureColor = gTexture[GH].Sample(gSampler, transformedUV.xy);
    
     // 入力が透明の場合は棄却
    if (textureColor.a <= 1e-5f) {
        discard;
    }
    
    PixelShaderOutput output;
    
    // ライティングが有効な場合------------------------------------------------------------------
    if (gMaterial[input.instanceID].lightingType != LightingType::NONE) {
    
        // カメラへのベクトルを求める
        float3 toEye = normalize(gCamera[gCameraIndex.value].position - input.worldPosition);
        // 格納用
        float3 specular = float3(0.0f, 0.0f, 0.0f);
        float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    
        // ディレクショナルライトの計算
        for (int i = 0; i < gDirectionalLightCount.value; i++) {
            CalcDirectionalLight(gDirectionalLight[i], gMaterial[input.instanceID], textureColor.rgb, input, toEye, diffuse, specular);
        }
    
        // ポイントライトの計算
        for (int j = 0; j < gPointLightCount.value; j++) {
            CalcPointLight(gPointLight[j], gMaterial[input.instanceID], textureColor.rgb, input, toEye, diffuse, specular);
        }
    
        // スポットライトの計算
        for (int k = 0; k < gSpotLightCount.value; k++) {
            CalcSpotLight(gSpotLight[k], gMaterial[input.instanceID], textureColor.rgb, input, toEye, diffuse, specular);
        }
        
        output.color.rgb = diffuse + specular;
        output.color.a = gMaterial[input.instanceID].color.a * textureColor.a;
        
        float3 reflectVector = reflect(-toEye, input.normal);
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectVector);
        output.color.rgb += gMaterial[input.instanceID].environmentCoef * environmentColor.rgb;
    
    }
    // ライティングが無効な場合------------------------------------------------------------------
    else {
        float3 cameraToPosition = normalize(input.worldPosition - gCamera[gCameraIndex.value].position);
        float3 reflectVector = reflect(cameraToPosition, input.normal);
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectVector);
    
        output.color = gMaterial[input.instanceID].color * textureColor;
        output.color.rgb += gMaterial[input.instanceID].environmentCoef * environmentColor.rgb;
    }
    
    
    // 出力が透明の場合は棄却
    if (output.color.a <= 1e-5f) {
        discard;
    }
    
    return output;
}
