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
        return output;
    }
    
    // billboardの場合はカメラの回転を掛ける
    Quaternion finalRot = particles[instanceID].rotation;
    //if(particles[instanceID].isBillboard) {
    //    finalRot = cameraInfo.cameraRotation.Multiply(particles[instanceID].localRotation);
    //}
    
    // ワールド行列の計算
    float4x4 worldMat;
    worldMat = AffineMatrix(
        particles[instanceID].scale,
        finalRot.ToFloat4(),
        particles[instanceID].position
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