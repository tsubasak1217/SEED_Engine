struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint instanceID : Index0;
};

struct LightingType
{
    static const int NONE = 0;
    static const int LAMBERT = 1;
    static const int HALF_LAMBERT = 2;
};

/*- 仮: 非線形を線形に変換する関数-*/
float DepthToLinear(float depth, float near, float far)
{
    float z = depth * 2.0f - 1.0f;
    float linearDepth = (2.0f * near * far) / (far + near - z * (far - near));
    return (linearDepth - near) / (far - near);
}

// フォーカスする深度や範囲
// 0.0 ~ 1.0の間でピントが合う中心深度
static const float focusDepth = 0.05f;
// ピントが影響を及ぼす直径
static const float focusRange = 1.0f;
// ピントが影響を及ぼす半径
static const float rangeRadius = focusRange * 0.65f;
//
static const float inverseRange = 1.0f / focusRange;
// 最近値、最遠値
static const float2 min_max =
{
    focusDepth - rangeRadius,
    focusDepth + rangeRadius
};

float CalcDepthData(float lenearDepth)
{
    float result;
    
    if (lenearDepth >= min_max.x && lenearDepth <= min_max.y)
    {
        result = (sin(4.71f + ((lenearDepth - min_max.x) * inverseRange) * 6.28f) + 1.0f) * 0.5f;
    }
    else
    {
        result = 0.0f;
    }
    
    return clamp(result,0.0f,1.0f);
}