#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

// パーティクル
RWStructuredBuffer<Particle> gParticles : register(u0, space0);

// フリーリスト関連
RWStructuredBuffer<uint> gFreeList : register(u1, space0); // フリーリスト(フリーなパーティクルの要素番号のリスト)
RWStructuredBuffer<int> gFreeListIndex : register(u2, space0); // 次の使用するフリーリストの要素番号

// 定数
ConstantBuffer<Float> gDeltaTime : register(b0); // DeltaTime

[numthreads(MAX_PARTICLE_COUNT, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    
    // 死んでいるパーティクルは処理しない
    if (gParticles[DTid.x].alive != 1) {
        return;
    }
    
    // 寿命に応じて媒介変数を計算
    float t = gParticles[DTid.x].currentTime / gParticles[DTid.x].lifeTime;
    
    // transformの更新
    gParticles[DTid.x].position += gParticles[DTid.x].direction * gParticles[DTid.x].speed * gDeltaTime.value;
    
    gParticles[DTid.x].localRotation =
    gParticles[DTid.x].localRotation.Multiply(
        Quaternion::AngleAxis(
            gParticles[DTid.x].rotateSpeed * gDeltaTime.value,
            gParticles[DTid.x].rotateAxis
        )
    );  

    gParticles[DTid.x].rotation = gParticles[DTid.x].localRotation;
    
    // 色の更新
    gParticles[DTid.x].color.a = 1.0f - t;
    
    // 時間の更新
    gParticles[DTid.x].currentTime += gDeltaTime.value;
    
    // パーティクルの寿命が尽きたら死ぬ
    gParticles[DTid.x].alive = gParticles[DTid.x].currentTime < gParticles[DTid.x].lifeTime;
    
    if(!gParticles[DTid.x].alive) {
    
        // スケールをゼロにして非表示にする
        gParticles[DTid.x].scale = float3(0.0f, 0.0f, 0.0f); 
    
        // フリーリストの更新
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
        
        if(freeListIndex + 1 < MAX_PARTICLE_COUNT) {
            // フリーリストにパーティクルのインデックスを追加
            gFreeList[freeListIndex + 1] = gParticles[DTid.x].particleIndex;
        } else {
            // 一応安全策
            InterlockedAdd(gFreeListIndex[0], -1);
        }
    }
}