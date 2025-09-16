#include "GPUParticle.hlsli"
#include "../Math/Math.hlsli"

/////////////////////////////////////// Transform //////////////////////////////////////////////

struct CameraInfo {
    float4x4 viewProjection; // View Projection Matrix
    Quaternion cameraRotation; // Camera Rotation
    float3 cameraPosition; // Camera Position
};


///////////////////////////////////////// Input ////////////////////////////////////////////////

struct VertexShaderInput {
    // VBV_0 (VertexData)
    float4 position : S0_V_POSITION0;
    float2 texcoord : S0_V_TEXCOORD0;
    float3 normal : S0_V_NORMAL0;
};


StructuredBuffer<Particle> particles : register(t0, space0);
ConstantBuffer<CameraInfo> cameraInfo : register(b0);

///////////////////////////////////////// main ////////////////////////////////////////////////

// Output
GPUParticleVSOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID, uint vertexID : SV_VertexID) {
    
    GPUParticleVSOutput output;
    output.instanceID = instanceID;

    if (particles[instanceID].alive == false) {
        output.particleTextureIdx = -1; // パーティクルが死んでいる場合はテクスチャインデックスを無効にする
        output.position = float4(0, 0, 0, 1); // 無効な位置にしておく
        return output;
    }
    
    // カメラの方向（カメラ座標系のZ軸 = -forward）
    float3 toCamera = normalize(cameraInfo.cameraPosition - particles[instanceID].position);

    // 常にカメラの方を向くための基底
    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, -toCamera));
    float3 forward = normalize(cross(right, up)); // or just toCamera

    // Z軸回転（パーティクルのローカル回転）
    float angle = particles[instanceID].rotation;
    float cosA = cos(angle);
    float sinA = sin(angle);

    // Z回転行列を right と up に適用（回転軸は forward）
    float3 rotatedRight = cosA * right + sinA * up;
    float3 rotatedUp = -sinA * right + cosA * up;

    // ワールド行列を構成
    float3 scale = particles[instanceID].scale;
    float3 pos = particles[instanceID].position;

    float4x4 worldMat = float4x4(
        float4(rotatedRight * scale.x, 0),
        float4(rotatedUp * scale.y, 0),
        float4(forward * scale.z, 0),
        float4(pos, 1)
    );
    
    // WVP行列の計算
    float4x4 wvpMat = mul(worldMat, cameraInfo.viewProjection);
    
    // Apply Transformation
    output.position = mul(input.position, wvpMat);
    output.worldPosition = mul(input.position, worldMat).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)worldMat));
    output.particleColor = particles[instanceID].color;
    output.particleTextureIdx = particles[instanceID].textureIdx;
    
    return output;
}