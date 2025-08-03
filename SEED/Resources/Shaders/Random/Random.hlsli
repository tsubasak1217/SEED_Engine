#ifndef RONDOM_INCLUDED
#define RONDOM_INCLUDED

struct RandomGenerator {
    uint state;

    // 初期化: uintシードを直接設定
    void Init(uint seed) {
        state = seed;
    }

    // floatをuintシードに変換して初期化
    void Init(float seedFloat) {
        state = asuint(seedFloat * 43758.5453f + 0.5f);
    }
    
    // Xorshift32による乱数生成
    uint Shift() {
        state ^= (state << 13);
        state ^= (state >> 17);
        state ^= (state << 5);
        return state;
    }

    // 0〜1のfloatを返す
    float Random01() {
        return (Shift() & 0x00FFFFFF) / 16777216.0f;
    }

    // min〜maxのfloatを返す
    float Random(float minVal, float maxVal) {
        return lerp(minVal, maxVal, Random01());
    }
    
    float2 Random2D(float minVal, float maxVal) {
        return float2(Random(minVal, maxVal), Random(minVal, maxVal));
    }
    
    float2 Random2D(float2 min, float2 max) {
        return float2(Random(min.x, max.x), Random(min.y, max.y));
    }
    
    float3 Random3D(float minVal, float maxVal) {
        return float3(Random(minVal, maxVal), Random(minVal, maxVal), Random(minVal, maxVal));
    }
    
    float3 Random3D(float3 min, float3 max) {
        return float3(Random(min.x, max.x), Random(min.y, max.y), Random(min.z, max.z));
    }
};

static RandomGenerator rand;

#endif