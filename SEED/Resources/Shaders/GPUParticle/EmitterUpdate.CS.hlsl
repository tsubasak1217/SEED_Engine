#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Random/Random.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

// オブジェクトリスト
StructuredBuffer<Emitter> gEmitters : register(t0, space0);
RWStructuredBuffer<Particle> gParticles : register(u0, space0); // ここにパーティクルを格納

// フリーリスト関連
RWStructuredBuffer<int> gFreeListIndex : register(u1, space0); // 次の使用するフリーリストの要素番号
StructuredBuffer<int> gFreeList : register(t1, space0);

// 定数
ConstantBuffer<Int> gEmitterCount : register(b0); // エミッターの数
ConstantBuffer<Float> gDeltaTime : register(b1); // DeltaTime
ConstantBuffer<Float> gCurrentTime : register(b2); // 現在の時間(乱数用)

///////////////////////////////////////////////////////////////////////
// エミッターの情報をもとにパーティクルを発生させる関数
///////////////////////////////////////////////////////////////////////
void Emit(Emitter emitter, float seed) {

    // 乱数生成器の初期化
    rand.Init(seed);

    for (uint i = 0; i < emitter.numEmitEvery; ++i) {

        // 末尾から減らしていく方式でフリーインデックスを取得
        int freeIndex;
        InterlockedAdd(gFreeListIndex[0], -1, freeIndex);

        // 範囲外で発生させられない場合,番号を戻して終了
        if (freeIndex < 0 || freeIndex >= MAX_PARTICLE_COUNT) {
            InterlockedAdd(gFreeListIndex[0], 1);
            continue;
        }
        
        // フリーリストからパーティクルのインデックスを取得
        uint freeListIndex = gFreeList[freeIndex];
        
        // パーティクルの初期化
        // translate
        float3 minPos = emitter.position - (emitter.emitRange * 0.5);
        float3 maxPos = emitter.position + (emitter.emitRange * 0.5);
        gParticles[freeListIndex].position = rand.Random3D(minPos, maxPos);
        
        // scale
        gParticles[freeListIndex].kScale = rand.Random3D(emitter.minScale, emitter.maxScale);
        gParticles[freeListIndex].scale = gParticles[freeListIndex].kScale;
        
        // 回転の初期化
        if (emitter.isBillboard) {
            gParticles[freeListIndex].isBillboard = true;
        }
        
        if (emitter.useRotateAxis) {
            gParticles[freeListIndex].rotateAxis = emitter.rotateAxis;
        } else {
            gParticles[freeListIndex].localRotation = Quaternion::ToQuaternion(rand.Random3D(emitter.minRotation, emitter.maxRotation));
            gParticles[freeListIndex].rotateAxis = normalize(rand.Random3D(-1.0f, 1.0f)); // ランダムな回転軸を生成
        }
        
        gParticles[freeListIndex].rotateSpeed = rand.Random(emitter.minRotateSpeed, emitter.maxRotateSpeed);
        
        // 基本方向とランダムな方向を組み合わせて最終的な方向を決定
        gParticles[freeListIndex].direction = emitter.baseDirection;
        float angleRange = 3.14f * clamp(emitter.directionRange, 0.0f, 1.0f);
        float theta = rand.Random(-angleRange, angleRange); // 水平回転
        float phi = rand.Random(-angleRange / 2.0f, angleRange / 2.0f); // 垂直回転 (範囲を制限)
        float3 rotateVec = CreateVector(theta, phi); // 球面上のベクトルを生成
        gParticles[freeListIndex].direction = Mul(rotateVec, Quaternion::DirectionToDirection(float3(1, 0, 0), gParticles[freeListIndex].direction)); // 回転を適用
    
        // その他
        gParticles[freeListIndex].speed = rand.Random(emitter.minSpeed, emitter.maxSpeed);
        gParticles[freeListIndex].lifeTime = rand.Random(emitter.minLifeTime, emitter.maxLifeTime);
        gParticles[freeListIndex].currentTime = 0;
        gParticles[freeListIndex].alive = true;
        gParticles[freeListIndex].color = emitter.color;
        
        // 書き込み
        gParticles[freeListIndex].particleIndex = freeListIndex;
        gParticles[freeListIndex].textureIdx = emitter.textureIndex;
    }
}


///////////////////////////////////////////////////////////////////////
// メイン関数
///////////////////////////////////////////////////////////////////////
[numthreads(MAX_EMITTER_COUNT, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {

    // エミッターの数を超えた場合は何もしない
    if (DTid.x >= gEmitterCount.value) {
        return;
    }

    // 終わっていれば処理しない
    if (gEmitters[DTid.x].alive != 1) {
        return;
    }
    
    // seedを計算
    float seed = (DTid.x * gCurrentTime.value) * gDeltaTime.value;
    
    // 時間が来たら発生
    if (gEmitters[DTid.x].currentTime >= gEmitters[DTid.x].interval) {
        
        // 繰り返すかどうかで処理を分岐
        Emit(gEmitters[DTid.x], seed);

    }
}