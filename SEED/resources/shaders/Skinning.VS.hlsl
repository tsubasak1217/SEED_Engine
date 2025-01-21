#include "Object3D.hlsli"

/////////////////////////////////////// Transform //////////////////////////////////////////////

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
    float4x4 worldInverseTranspose;
};

/////////////////////////////////////// Skinning //////////////////////////////////////////////

struct Well {
    float4x4 skeletonSpaceMatrix;
    float4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Skinned {
    float4 position;
    float3 normal;
};

/////////////////////////////////////// VsInput /////////////////////////////////////////////

struct VertexShaderInput {
    // VBV_0 (VertexData)
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    
    // VBV_1 (OffsetData)
    int indexOffset : INDEX_OFFSET0;
    int meshOffset : MESH_OFFSET0;
    int jointIndexOffset : JOINT_INDEX_OFFSET0;
    int jointInterval : JOINT_INTERVAL0;
    int interval : INTERVAL0; //line->2,triangle->3,quad->4
    
    // VBV_2 (Animation)
    float4 weight : WEIGHT0;
    int4 jointIndex : JOINT_INDEX0;
};


StructuredBuffer<TransformationMatrix> instanceData : register(t0, space0);
StructuredBuffer<Well> gMatrixPalette : register(t1, space0);


/////////////////////////////////////// Methods //////////////////////////////////////////////

// Skinning
Skinned Skinning(VertexShaderInput input,int offset) {
    Skinned skinned;
    
    if (input.jointIndex.x + input.jointIndex.y + input.jointIndex.z + input.jointIndex.w < 0) {
        skinned.position = input.position;
        skinned.normal = input.normal;
        return skinned;
    }
    
    // calculate skinned position
    skinned.position = mul(input.position, gMatrixPalette[input.jointIndex.x + offset].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.jointIndex.y + offset].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.jointIndex.z + offset].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.jointIndex.w + offset].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f;
    
    // calculate skinned normal
    skinned.normal = mul(input.normal, (float3x3) gMatrixPalette[input.jointIndex.x + offset].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.jointIndex.y + offset].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.jointIndex.z + offset].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.jointIndex.w + offset].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
    skinned.normal = normalize(skinned.normal); // normalize
    
    return skinned;
}


///////////////////////////////////////// main ////////////////////////////////////////////////

// Output
VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID, uint vertexID : SV_VertexID) {
    
    VertexShaderOutput output;
    int offset = input.jointIndexOffset + input.jointInterval * instanceID;
    Skinned skinned = Skinning(input,offset);
    int index = 0;
    
    // Caluculate InstanceID
    if (input.interval == 0) {// model
        index = instanceID + input.indexOffset;
    } else {// primitive
        index = instanceID + input.indexOffset + (vertexID / input.interval);
    }
    
    // Apply Transformation
    output.position = mul(skinned.position, instanceData[index].WVP);
    output.worldPosition = mul(input.position, instanceData[index].world).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(skinned.normal, (float3x3) instanceData[index].worldInverseTranspose));
    
    // Caluculate MaterialID
    if (input.interval == 0) {// model
        output.instanceID = input.meshOffset + instanceID;
    } else {// primitive
        output.instanceID = input.meshOffset + instanceID + (vertexID / input.interval);
    }
    
    
    return output;
}