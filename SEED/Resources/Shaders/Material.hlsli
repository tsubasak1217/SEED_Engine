#ifndef MATERIAL_HLSL
#define MATERIAL_HLSL

struct Material {
    float4 color;
    float4x4 uvTransform;
    int lightingType;
    int GH;
    float shinines;
};

#endif