#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

// パーティクル
RWStructuredBuffer<Particle> gParticles : register(u0, space0);

// フリーリスト関連
RWStructuredBuffer<uint> gFreeList : register(u1, space0); // フリーリスト(フリーなパーティクルの要素番号のリスト)
RWStructuredBuffer<int> gFreeListIndex : register(u2, space0); // 次の使用するフリーリストの要素番号

// パーティクルの数
RWStructuredBuffer<int> gParticleCount : register(u3, space0); // 次の使用するフリーリストの要素番号

// 定数
ConstantBuffer<Float> gDeltaTime : register(b0); // DeltaTime
ConstantBuffer<Int> gMaxParticleCount : register(b1); // 最大パーティクル数

[numthreads(1024, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    
    int index = DTid.x;
    
    // 死んでいるパーティクルは処理しない
    if (gParticles[index].alive != 1) {
        return;
    }
    
    // 範囲外のアクセスを防ぐ
    if(index < 0) {
        return;
    }
    
    // 寿命に応じて媒介変数を計算
    float t = gParticles[index].currentTime / gParticles[index].lifeTime;
    
    // transformの更新
    gParticles[index].position += gParticles[index].direction * gParticles[index].speed * gDeltaTime.value;
    gParticles[index].rotation += gParticles[index].rotateSpeed * gDeltaTime.value;
    
    // 色の更新
    gParticles[index].color.a = 1.0f - t;
    
    // 時間の更新
    gParticles[index].currentTime += gDeltaTime.value;
    
    // パーティクルの寿命が尽きたら死ぬ
    gParticles[index].alive = gParticles[index].currentTime < gParticles[index].lifeTime;
    
    if(!gParticles[index].alive) {
    
        // スケールをゼロにして非表示にする
        gParticles[index].scale = float3(0.0f, 0.0f, 0.0f); 
    
        // フリーリストの更新
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
        InterlockedAdd(gParticleCount[0], -1);
        
        if (freeListIndex + 1 < gMaxParticleCount.value) {
            // フリーリストにパーティクルのインデックスを追加
            gFreeList[freeListIndex + 1] = gParticles[index].particleIndex;
        } else {
            // 一応安全策
            InterlockedAdd(gFreeListIndex[0], -1);
        }
    }
}