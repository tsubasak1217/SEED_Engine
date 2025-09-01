#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Random/Random.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

// オブジェクトリスト
ConstantBuffer<Emitter> gEmitter : register(b0, space0);
RWStructuredBuffer<Particle> gParticles : register(u0, space0); // ここにパーティクルを格納

// フリーリスト関連
RWStructuredBuffer<int> gFreeListIndex : register(u1, space0); // 次の使用するフリーリストの要素番号
StructuredBuffer<int> gFreeList : register(t0, space0);

// パーティクルの数
RWStructuredBuffer<int> gParticleCount : register(u2, space0); // 次の使用するフリーリストの要素番号

// 定数
ConstantBuffer<Float> gDeltaTime : register(b1); // DeltaTime(乱数用)
ConstantBuffer<Float> gTotalTime : register(b2); // 時刻(乱数用)
ConstantBuffer<Int> gMaxParticleCount : register(b3); // 最大パーティクル数

///////////////////////////////////////////////////////////////////////
// エミッターの情報をもとにパーティクルを発生させる関数
///////////////////////////////////////////////////////////////////////
void Emit(float seed) {

    // 末尾から減らしていく方式でフリーインデックスを取得
    int freeIndex;
    InterlockedAdd(gFreeListIndex[0], -1, freeIndex);

    // 範囲外で発生させられない場合,番号を戻して終了
    if (freeIndex < 0 || freeIndex >= gMaxParticleCount.value) {
        InterlockedAdd(gFreeListIndex[0], 1);
        return;
    }
    
    // 乱数生成器の初期化
    Random rand;
    rand.Init(seed);
    
    // フリーリストからパーティクルのインデックスを取得
    uint freeListIndex = gFreeList[freeIndex];
    
    // パーティクルの初期化
    // translate
    float3 minPos = gEmitter.position - (gEmitter.emitRange * 0.5);
    float3 maxPos = gEmitter.position + (gEmitter.emitRange * 0.5);
    gParticles[freeListIndex].position = rand.Get3D(minPos, maxPos);
    
    // scale
    gParticles[freeListIndex].kScale = rand.Get3D(gEmitter.minScale, gEmitter.maxScale);
    gParticles[freeListIndex].scale = gParticles[freeListIndex].kScale;
    
    // 回転の初期化
    gParticles[freeListIndex].rotation = rand.Get1D(gEmitter.minRotation, gEmitter.maxRotation);
    gParticles[freeListIndex].rotateSpeed = rand.Get1D(gEmitter.minRotateSpeed, gEmitter.maxRotateSpeed);
    
    // 発射方向を設定
    gParticles[freeListIndex].direction = gEmitter.baseDirection;
    float angleRange = PI * clamp(gEmitter.angleRange, 0.0f, 2.0f);
    float theta = rand.Get1D(-angleRange, angleRange); // 水平回転
    float phi = rand.Get1D(-angleRange, angleRange); // 垂直回転 (範囲を制限)
    float3 rotateVec = CreateVector(theta, phi); // 球面上のベクトルを生成
    gParticles[freeListIndex].direction = Mul(rotateVec, Quaternion::DirectionToDirection(float3(1, 0, 0), gParticles[freeListIndex].direction)); // 回転を適用
    
    // その他
    gParticles[freeListIndex].color = gEmitter.color;
    gParticles[freeListIndex].speed = rand.Get1D(gEmitter.minSpeed, gEmitter.maxSpeed);
    gParticles[freeListIndex].lifeTime = rand.Get1D(gEmitter.minLifeTime, gEmitter.maxLifeTime);
    gParticles[freeListIndex].currentTime = 0;
    gParticles[freeListIndex].alive = true;
    gParticles[freeListIndex].textureIdx = gEmitter.textureIndex;
    gParticles[freeListIndex].particleIndex = freeListIndex;
    
    // パーティクルの数を増やす
    InterlockedAdd(gParticleCount[0], 1);
}


///////////////////////////////////////////////////////////////////////
// メイン関数
///////////////////////////////////////////////////////////////////////
[numthreads(1024, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    
    // 発生数を超えた場合は何もしない
    if (DTid.x >= gEmitter.numEmitEvery) {
        return;
    }
    
    // 乱数用のシードを生成
    float seed = gTotalTime.value + gDeltaTime.value * DTid.x;
    
    // 出現させる
    Emit(seed);
}