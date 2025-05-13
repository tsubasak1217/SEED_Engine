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
ConstantBuffer<OffsetData> offsetData : register(b0);
// 

///////////////////////////////////////// Output ////////////////////////////////////////////////

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    uint3 groupID : SV_GroupID,// vec3,threadNo
    uint groupIndex : SV_GroupIndex,// linerIndex
    out vertices MeshShaderOutput verts[64],
    out indices uint3 inds[128]
    ) {
    　
    // Simulate instanceID and vertexID
    uint localInstanceID = groupID.y;
    uint globalInstanceID;
    uint index = indices[groupIndex];
    VertexData vertex = vertices[index];
    
    // グローバルなインスタンスIDの計算
    if (offsetData.interval == 0) {
        globalInstanceID = offsetData.meshOffset + localInstanceID;
    } else {
        globalInstanceID = offsetData.meshOffset + localInstanceID + (groupID.x / offsetData.interval);
    }

    // Apply Transformation
    TransformationMatrix mat = transformDatas[globalInstanceID];
    MeshShaderOutput output;
    output.position = mul(vertex.position, mat.WVP);
    output.worldPosition = mul(vertex.position, mat.world).xyz;
    output.texcoord = vertex.texcoord;
    output.normal = normalize(mul(vertex.normal, (float3x3) mat.worldInverseTranspose));

}