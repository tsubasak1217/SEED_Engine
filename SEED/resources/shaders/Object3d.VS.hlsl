#include "Object3D.hlsli"

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
};

struct INT {
    int value;
};


StructuredBuffer<TransformationMatrix> InstanceData : register(t0, space0);

struct VertexShaderInput {
    // VBV_0
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    
    // VBV_1
    int indexOffset : INDEX_OFFSET0;
    int meshOffset : MESH_OFFSET0;
    int interval : INTERVAL0; //line->2,triangle->3,quad->4
};

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID, uint vertexID : SV_VertexID) {
    
    VertexShaderOutput output;
    int index = 0;
    if (input.interval == 0) {
        index = instanceID + input.indexOffset;
    } else {
        index = instanceID + input.indexOffset + (vertexID / input.interval);
    }
    
    output.position = mul(input.position, InstanceData[index].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) InstanceData[index].world));
    
    if (input.interval == 0) {
        output.instanceID = input.meshOffset + instanceID;
    } else {
        output.instanceID = input.meshOffset + instanceID + (vertexID / input.interval);
    }
    
    
    return output;
}