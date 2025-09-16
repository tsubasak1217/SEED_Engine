#ifndef MATERIAL_HLSLI
#define MATERIAL_HLSLI

struct Material {
    float4 color;
    float4x4 uvTransform;
    int lightingType;
    int GH;
    float shinines;
    float environmentCoef;
};

#endif