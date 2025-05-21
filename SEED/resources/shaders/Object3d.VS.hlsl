#include "Object3d.hlsli"

/////////////////////////////////////// Transform //////////////////////////////////////////////

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
    float4x4 worldInverseTranspose;
};

///////////////////////////////////////// Input ////////////////////////////////////////////////

struct VertexShaderInput {
    // VBV_0 (VertexData)
    float4 position : S0_V_POSITION0;
    float2 texcoord : S0_V_TEXCOORD0;
    float3 normal : S0_V_NORMAL0;
    
    // VBV_1 (OffsetData)
    int indexOffset : S1_I_INDEX_OFFSET0;
    int meshOffset : S1_I_MESH_OFFSET0;
    int jointIndexOffset : S1_I_JOINT_INDEX_OFFSET0;
    int jointInterval : S1_I_JOINT_INTERVAL0;
    int interval : S1_I_INTERVAL0; //line->2,triangle->3,quad->4
};


StructuredBuffer<TransformationMatrix> transforms : register(t0, space0);


///////////////////////////////////////// main ////////////////////////////////////////////////

// Output
MeshShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID, uint vertexID : SV_VertexID) {
    
    MeshShaderOutput output;
    int transformIdx = 0;
    
    // Caluculate InstanceIdx
    if (input.interval == 0) { // model
        transformIdx = instanceID + input.indexOffset;
    } else { // primitive
        transformIdx = instanceID + input.indexOffset + (vertexID / input.interval);
    }
    
    // Apply Transformation
    output.position = mul(input.position, transforms[transformIdx].WVP);
    output.worldPosition = mul(input.position, transforms[transformIdx].world).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) transforms[transformIdx].worldInverseTranspose));
    
    // Caluculate MaterialIdx
    if (input.interval == 0) { // model
        output.instanceID = input.meshOffset + instanceID;
    } else { // primitive
        output.instanceID = input.meshOffset + instanceID + (vertexID / input.interval);
    }
    
    
    return output;
}