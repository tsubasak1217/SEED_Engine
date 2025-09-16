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
        output.particleTextureIdx = -1; // �p�[�e�B�N��������ł���ꍇ�̓e�N�X�`���C���f�b�N�X�𖳌��ɂ���
        output.position = float4(0, 0, 0, 1); // �����Ȉʒu�ɂ��Ă���
        return output;
    }
    
    // �J�����̕����i�J�������W�n��Z�� = -forward�j
    float3 toCamera = normalize(cameraInfo.cameraPosition - particles[instanceID].position);

    // ��ɃJ�����̕����������߂̊��
    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, -toCamera));
    float3 forward = normalize(cross(right, up)); // or just toCamera

    // Z����]�i�p�[�e�B�N���̃��[�J����]�j
    float angle = particles[instanceID].rotation;
    float cosA = cos(angle);
    float sinA = sin(angle);

    // Z��]�s��� right �� up �ɓK�p�i��]���� forward�j
    float3 rotatedRight = cosA * right + sinA * up;
    float3 rotatedUp = -sinA * right + cosA * up;

    // ���[���h�s����\��
    float3 scale = particles[instanceID].scale;
    float3 pos = particles[instanceID].position;

    float4x4 worldMat = float4x4(
        float4(rotatedRight * scale.x, 0),
        float4(rotatedUp * scale.y, 0),
        float4(forward * scale.z, 0),
        float4(pos, 1)
    );
    
    // WVP�s��̌v�Z
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