#pragma once
#include <list>
#include <memory>
#include <initializer_list>
#include "SEED.h"
#include "Range1D.h"
#include "Range3D.h"
#include "AccelerarionField.h"

// particles
#include "BaseParticle.h"
#include "RadialParticle.h"

enum class ParticleType : int32_t{
    kRadial = 0,
};

class ParticleManager{

private:
    ParticleManager() = default;
    // コピー禁止
    ParticleManager(const ParticleManager&) = delete;
    void operator=(const ParticleManager&) = delete;

public:
    ~ParticleManager();
    static ParticleManager* GetInstance();
    static void Update();
    static void Draw();

public:

    /// <summary>
    /// 加速フィールドを作成する
    /// </summary>
    /// <param name="range">フィールドの範囲</param>
    /// <param name="force">加速度</param>
    static void CreateAccelerationField(const Range3D& range, const Vector3& force);

    /// <summary>
    /// パーティクルを発生させる
    /// </summary>
    /// <param name="type">パーティクルの種類</param>
    /// <param name="positionRange">発生範囲</param>
    /// <param name="radiusRange">大きさの範囲</param>
    /// <param name="speedRange">スピードの範囲</param>
    /// <param name="lifeTime">寿命時間</param>
    /// <param name="colors">発生させる色の一覧</param>
    /// <param name="count">一度に発生させる数</param>
    /// <param name="blendMode">ブレンドモード</param>
    static void Emit(
        ParticleType type,
        const Range3D& positionRange,
        const Range1D& radiusRange,
        const Range1D& speedRange,
        float lifeTime,
        const std::initializer_list<Vector4>& colors,
        int32_t count = 1,
        BlendMode blendMode = BlendMode::ADD
    );

    /// <summary>
    /// パーティクルを発生させる(時間経過で)
    /// </summary>
    /// <param name="type">パーティクルの種類</param>
    /// <param name="positionRange">発生範囲</param>
    /// <param name="radiusRange">大きさの範囲</param>
    /// <param name="speedRange">スピードの範囲</param>
    /// <param name="lifeTime">寿命時間</param>
    /// <param name="colors">発生させる色の一覧</param>
    /// <param name="interval">発生間隔</param>
    /// <param name="count">一度に発生させる数</param>
    /// <param name="blendMode">ブレンドモード</param>
    static void Emit(
        ParticleType type,
        const Range3D& positionRange,
        const Range1D& radiusRange,
        const Range1D& speedRange,
        float lifeTime,
        const std::initializer_list<Vector4>& colors,
        float interval,
        int32_t count = 1,
        BlendMode blendMode = BlendMode::ADD
    );

private:

    // パーティクルと加速フィールドの衝突判定
    void CollisionParticle2Field();

private:

    static ParticleManager* instance_;

private:

    bool isFieldActive_;
    std::list<std::unique_ptr<BaseParticle>> particles_;
    std::list<std::unique_ptr<AccelerationField>> accelerationFields_;
};