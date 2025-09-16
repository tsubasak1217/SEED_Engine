#ifndef RONDOM_INCLUDED
#define RONDOM_INCLUDED

struct Random {

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
    float Get01() {
        return (Shift() & 0x00FFFFFF) / 16777216.0f;
    }

    // min〜maxのfloatを返す
    float Get1D(float minVal, float maxVal) {
        return lerp(minVal, maxVal, Get01());
    }
    
    float2 Get2D(float minVal, float maxVal) {
        return float2(Get1D(minVal, maxVal), Get1D(minVal, maxVal));
    }
    
    float2 Get2D(float2 min, float2 max) {
        return float2(Get1D(min.x, max.x), Get1D(min.y, max.y));
    }
    
    float3 Get3D(float minVal, float maxVal) {
        return float3(Get1D(minVal, maxVal), Get1D(minVal, maxVal), Get1D(minVal, maxVal));
    }
    
    float3 Get3D(float3 min, float3 max) {
        return float3(Get1D(min.x, max.x), Get1D(min.y, max.y), Get1D(min.z, max.z));
    }
};

#endif