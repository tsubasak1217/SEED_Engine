#include "Object3D.hlsli"
#include "Skybox.hlsli"

/////////////////////////////////////// Transform //////////////////////////////////////////////

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
    float4x4 worldInverseTranspose;
};

///////////////////////////////////////// Input ////////////////////////////////////////////////

struct VertexShaderInput {
    // VBV_0 (VertexData)
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    
    // VBV_1 (OffsetData)
    int indexOffset : INDEX_OFFSET0;
    int meshOffset : MESH_OFFSET0;
    int jointIndexOffset : JOINT_INDEX_OFFSET0;
    int interval : INTERVAL0; //line->2,triangle->3,quad->4
};


StructuredBuffer<TransformationMatrix> transformDatas : register(t0, space0);


SkyboxVSOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID, uint vertexID : SV_VertexID) {
    SkyboxVSOutput output;
    
    // Caluculate InstanceID
    int index = 0;
    index = instanceID + input.indexOffset;
    
    // Apply Transformation
    output.position = mul(input.position, transformDatas[index].WVP).xyww;
    output.worldPosition = mul(input.position, transformDatas[index].world).xyz;
    output.texcoord = input.position.xyz;
    output.normal = normalize(mul(input.normal, (float3x3) transformDatas[index].worldInverseTranspose));
    
    // Caluculate MaterialID
    output.instanceID = input.meshOffset + instanceID;
    
    return output;
}