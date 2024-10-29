#include "ParticleShader.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 world;
};

struct INT
{
    int value;
};

StructuredBuffer<TransformationMatrix> InstanceData : register(t0);
StructuredBuffer<INT> keyIndices : register(t1);
ConstantBuffer<INT> numElements : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint vertexID : SV_VertexID)
{
    VertexShaderOutput output;
    int instanceID;
    
    for (instanceID = 0; instanceID < numElements.value;)
    {
        if (vertexID >= keyIndices[instanceID].value)
        {
            instanceID++;
        }
        else
        {
            break;
        }
    }
    
    output.position = mul(input.position, InstanceData[instanceID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) InstanceData[instanceID].world));
    output.instanceID = instanceID;
    return output;
}