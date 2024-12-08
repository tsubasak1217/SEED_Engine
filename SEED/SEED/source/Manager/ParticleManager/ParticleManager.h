#pragma once
#include <list>
#include <memory>
#include <initializer_list>
#include "Range1D.h"
#include "Range3D.h"
#include "AccelerarionField.h"
#include "Emitter.h" 

// particles
#include "BaseParticle.h"
#include "RadialParticle.h"



class ParticleManager{

private:
    ParticleManager() = default;
    // コピー禁止
    ParticleManager(const ParticleManager&) = delete;
    void operator=(const ParticleManager&) = delete;

public:
    ~ParticleManager();
    static ParticleManager* GetInstance();
    static void Initialize();
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
    /// エミッターを追加する
    /// </summary>
    static void AddEmitter(const Emitter& emitter);

    /// <summary>
    /// エミッターを追加する(直接)
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
    static void AddEmitter(
        EmitType emitType,
        ParticleType particleType,
        const Vector3& center,
        const Vector3& emitRange,
        const Range1D& radiusRange,
        const Range1D& speedRange,
        const Range1D& lifeTimeRange,
        const std::initializer_list<Vector4>& colors,
        float interval,
        int32_t numEmitEvery = 1,
        BlendMode blendMode = BlendMode::ADD,
        int32_t numEmitMax = 1
    );

private:

    /// <summary>
    /// エミッターの情報からパーティクルを発生させる
    /// </summary>
    static void Emit(Emitter& emitter);


    /// <summary>
    /// パーティクルを発生させる
    /// </summary>
    static void Emit(
        ParticleType type,
        const Range3D& positionRange,
        const Range1D& radiusRange,
        const Range1D& speedRange,
        const Range1D& lifeTimeRange,
        const std::vector<Vector4>& colors,
        int32_t numEmit = 1,
        BlendMode blendMode = BlendMode::ADD
    );

    /// <summary>
    /// ImGuiでエミッターの編集
    /// </summary>
    void EditEmitter();

private:// ファイルの入出力

    // jsonファイルに保存
    void SaveToJson();
    // jsonファイルから読み込み
    void LoadFromJson();
    // ホットリロード
    void HotReload();

private:

    // パーティクルと加速フィールドの衝突判定
    void CollisionParticle2Field();

private:

    static ParticleManager* instance_;

private:

    bool isFieldActive_;
    bool isFieldVisible_ = true;

    std::list<std::unique_ptr<Emitter>> emitters_;
    std::list<std::unique_ptr<BaseParticle>> particles_;
    std::list<std::unique_ptr<AccelerationField>> accelerationFields_;
};