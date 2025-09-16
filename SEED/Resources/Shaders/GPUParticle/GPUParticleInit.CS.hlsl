#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0, space0);
RWStructuredBuffer<uint> gFreeList : register(u1, space0); // フリーリスト(フリーなパーティクルの要素番号のリスト)
RWStructuredBuffer<int> gFreeListIndex : register(u2, space0); // 次の使用するフリーリストの要素番号
ConstantBuffer<Int> gMaxParticleCount : register(b0); // 最大パーティクル数

[numthreads(1024, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    
    gParticles[DTid.x] = (Particle)0;
    gFreeList[DTid.x] = DTid.x; // フリーリストにパーティクルのインデックスを追加
    
    if(DTid.x == 0) {
        // フリーリストのインデックスを更新
        gFreeListIndex[0] = gMaxParticleCount.value - 1; // 最後の要素を指す
    }
}