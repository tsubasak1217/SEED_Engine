#ifndef OBJECT3D_INCLUDED
#define OBJECT3D_INCLUDED

struct MeshShaderOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : WORLDPOSITION0;
    uint instanceID : Index0;
};

struct VertexShaderOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : WORLDPOSITION0;
    uint instanceID : Index0;
};

struct SkyBoxVSOutput {
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : WORLDPOSITION0;
    uint instanceID : Index0;
};

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

struct PixelShaderOutput_DepthWrite {
    float4 color : SV_TARGET0;
    float depth : SV_Depth;
};

struct Int {
    int value;
};

struct Float {
    float value;
};

struct Float4x4 {
    float4x4 value;
};

#endif// Object3D_INCLUDED