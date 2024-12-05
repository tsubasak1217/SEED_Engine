#include "Object3D.hlsli"

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
};


StructuredBuffer<TransformationMatrix> instanceData : register(t0, space0);

struct VertexShaderInput {
    // VBV_0 (VertexData)
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    
    // VBV_1 (OffsetData)
    int indexOffset : INDEX_OFFSET0;
    int meshOffset : MESH_OFFSET0;
    int interval : INTERVAL0; //line->2,triangle->3,quad->4
};


// Output
VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID, uint vertexID : SV_VertexID) {
    
    VertexShaderOutput output;
    int index = 0;
    
    // Caluculate InstanceID
    if (input.interval == 0) {// model
        index = instanceID + input.indexOffset;
    } else {// primitive
        index = instanceID + input.indexOffset + (vertexID / input.interval);
    }
    
    // Apply Transformation
    output.position = mul(input.position, instanceData[index].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) instanceData[index].world));
    
    // Caluculate MaterialID
    if (input.interval == 0) {// model
        output.instanceID = input.meshOffset + instanceID;
    } else {// primitive
        output.instanceID = input.meshOffset + instanceID + (vertexID / input.interval);
    }
    
    
    return output;
}