#pragma once

struct SkyboxVSOutput {
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : WORLDPOSITION0;
    uint instanceID : Index0;
};