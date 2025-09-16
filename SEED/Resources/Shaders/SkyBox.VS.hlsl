#include "Object3D.hlsli"

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
    float4x4 worldInverseTranspose;
};

struct VertexShaderInput {
    // VBV_0 (VertexData)
    float4 position : S0_V_POSITION0;
    float2 texcoord : S0_V_TEXCOORD0;
    float3 normal : S0_V_NORMAL0;
    
    // VBV_1 (OffsetData)
    int indexOffset : S1_I_INDEX_OFFSET0;
    int meshOffset : S1_I_MESH_OFFSET0;
    int jointIndexOffset : S1_I_JOINT_INDEX_OFFSET0;
    int interval : S1_I_INTERVAL0; //line->2,triangle->3,quad->4
};

ConstantBuffer<Int> cameraIndexOffset : register(b0);
StructuredBuffer<TransformationMatrix> transforms : register(t0, space0);


SkyBoxVSOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID, uint vertexID : SV_VertexID) {
    SkyBoxVSOutput output;
    
    // Caluculate InstanceID
    int index = 0;
    index = instanceID + input.indexOffset + cameraIndexOffset.value;
    
    // Apply Transformation
    output.position = mul(input.position, transforms[index].WVP).xyww;
    output.worldPosition = mul(input.position, transforms[index].world).xyz;
    output.texcoord = input.position.xyz;
    output.normal = normalize(mul(input.normal, (float3x3) transforms[index].worldInverseTranspose));
    
    // Caluculate MaterialID
    output.instanceID = input.meshOffset + instanceID;
    
    return output;
}