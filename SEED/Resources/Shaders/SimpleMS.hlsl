#include "Object3D.hlsli"

/////////////////////////////////////// Transform //////////////////////////////////////////////

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
    float4x4 worldInverseTranspose;
};

///////////////////////////////////////// Input ////////////////////////////////////////////////

struct VertexData {
    float4 position;
    float2 texcoord;
    float3 normal;
};

struct OffsetData {
    int indexOffset;
    int meshOffset;
    int jointIndexOffset;
    int interval;
};

StructuredBuffer<TransformationMatrix> transformDatas : register(t0, space0);
StructuredBuffer<VertexData> vertices : register(t1, space0);
StructuredBuffer<int> indices : register(t2, space0);
StructuredBuffer<OffsetData> offsetDatas : register(t3, space0);


///////////////////////////////////////// Output ////////////////////////////////////////////////

struct MeshVertexOutput {
    float4 position : SV_Position;
    float3 worldPosition : TEXCOORD1;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint instanceID : INSTANCE_ID;
};

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint groupIndex : SV_GroupIndex,
    out vertices MeshVertexOutput verts[3],
    out indices uint3 inds[1]
    ) {
    
    // Simulate instanceID and vertexID
    uint globalInstanceID;
    uint index = indices[groupIndex];
    VertexData vertex = vertices[index]; // Or map by your own indexing
    OffsetData offsetData = offsetDatas[index];
    
    // calc instanceID
    if (offsetData.interval == 0) {
        globalInstanceID = offsetData.meshOffset + groupID.x;
    } else {
        globalInstanceID = offsetData.meshOffset + groupID.x + (groupIndex / offsetData.interval);
    }

    // Apply Transformation
    TransformationMatrix mat = transformDatas[groupIndex];
    MeshVertexOutput output;
    output.position = mul(input.position, mat.WVP);
    output.worldPosition = mul(input.position, mat.world).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) mat.worldInverseTranspose));


    // Simple triangle output (1 triangle with 3 vertices)
    verts[0] = output;
    verts[1] = output;
    verts[2] = output;
    inds[0] = uint3(0, 1, 2);
}