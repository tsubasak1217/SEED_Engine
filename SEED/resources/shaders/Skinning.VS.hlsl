#include "Object3D.hlsli"

/////////////////////////////////////// Transform //////////////////////////////////////////////

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
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
    int interval : INTERVAL0; //line->2,triangle->3,quad->4
    
    // VBV_2 (Animation)
    bool isSkinning : IS_SKINNING0;
    float4 weight : WEIGHT0;
    int4 jointIndex : JOINT_INDEX0;
};


StructuredBuffer<TransformationMatrix> instanceData : register(t0, space0);
StructuredBuffer<Well> gMatrixPalette : register(t1, space0);


/////////////////////////////////////// Methods //////////////////////////////////////////////

// Skinning
Skinned Skinning(VertexShaderInput input) {
    Skinned output;
    
    // early return
    if (!input.isSkinning) {
        output.position = input.position;
        output.normal = input.normal;
        return output;
    }
    
    // calculate skinned position
    output.position = mul(input.position, gMatrixPalette[input.jointIndex.x].skeletonSpaceMatrix) * input.weight.x;
    output.position += mul(input.position, gMatrixPalette[input.jointIndex.y].skeletonSpaceMatrix) * input.weight.y;
    output.position += mul(input.position, gMatrixPalette[input.jointIndex.z].skeletonSpaceMatrix) * input.weight.z;
    output.position += mul(input.position, gMatrixPalette[input.jointIndex.w].skeletonSpaceMatrix) * input.weight.w;
    output.position.w = 1.0f;
    
    // calculate skinned normal
    output.normal = mul(input.normal, (float3x3) gMatrixPalette[input.jointIndex.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
    output.normal += mul(input.normal, (float3x3) gMatrixPalette[input.jointIndex.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
    output.normal += mul(input.normal, (float3x3) gMatrixPalette[input.jointIndex.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
    output.normal += mul(input.normal, (float3x3) gMatrixPalette[input.jointIndex.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
    output.normal = normalize(output.normal);// normalize
    
    return output;
}


///////////////////////////////////////// main ////////////////////////////////////////////////

// Output
VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID, uint vertexID : SV_VertexID) {
    
    VertexShaderOutput output;
    Skinned skinned = Skinning(input);
    int index = 0;
    
    // Caluculate InstanceID
    if (input.interval == 0) {// model
        index = instanceID + input.indexOffset;
    } else {// primitive
        index = instanceID + input.indexOffset + (vertexID / input.interval);
    }
    
    // Apply Transformation
    output.position = mul(skinned.position, instanceData[index].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(skinned.normal, (float3x3) instanceData[index].world));
    
    // Caluculate MaterialID
    if (input.interval == 0) {// model
        output.instanceID = input.meshOffset + instanceID;
    } else {// primitive
        output.instanceID = input.meshOffset + instanceID + (vertexID / input.interval);
    }
    
    
    return output;
}