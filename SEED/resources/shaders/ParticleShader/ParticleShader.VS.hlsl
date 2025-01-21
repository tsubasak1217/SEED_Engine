#include "ParticleShader.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 world;
    float4x4 worldInverseTranspose;
};

struct INT
{
    int value;
};

StructuredBuffer<TransformationMatrix> InstanceData : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, InstanceData[instanceID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) InstanceData[instanceID].world));
    output.instanceID = instanceID;
    return output;
}