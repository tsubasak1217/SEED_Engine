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
Texture2D<float4> gTexture[256] : register(t5, space0);
SamplerState gSampler : register(s0);


PixelShaderOutput_DepthWrite main(MeshShaderOutput input) {
    int GH = gMaterial[input.instanceID].GH;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial[input.instanceID].uvTransform);
    float4 textureColor = gTexture[GH].Sample(gSampler, transformedUV.xy);
    
     // 入力が透明の場合は棄却
    if (textureColor.a <= 1e-5f) {
        discard;
    }
    
    PixelShaderOutput_DepthWrite output;
    
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
    
    }
    // ライティングが無効な場合------------------------------------------------------------------
    else {
        output.color = gMaterial[input.instanceID].color * textureColor;
    }
    
    
    // 出力が透明の場合は棄却
    if (output.color.a <= 1e-5f) {
        discard;
    }
    
    // 書き込んだら最前面にする
    output.depth = 0.0f;
    
    return output;
}
