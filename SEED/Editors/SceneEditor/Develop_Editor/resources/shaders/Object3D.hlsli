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

/*- ��: ����`����`�ɕϊ�����֐�-*/
float DepthToLinear(float depth, float near, float far)
{
    float z = depth * 2.0f - 1.0f;
    float linearDepth = (2.0f * near * far) / (far + near - z * (far - near));
    return (linearDepth - near) / (far - near);
}

// �t�H�[�J�X����[�x��͈�
// 0.0 ~ 1.0�̊ԂŃs���g���������S�[�x
static const float focusDepth = 0.2f;
// �s���g���e�����y�ڂ����a
static const float focusRange = 6.0f;
// �s���g���e�����y�ڂ����a
static const float rangeRadius = focusRange * 0.65f;
//
static const float inverseRange = 1.0f / focusRange;
// �ŋߒl�A�ŉ��l
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