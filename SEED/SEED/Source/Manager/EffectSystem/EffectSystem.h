#pragma once
#include <list>
#include <memory>
#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/AccelerarionField.h>

// emitters
#include <SEED/Source/Object/Particle/Emitter/Emitter.h> 
#include <SEED/Source/Object/Particle/Emitter/Emitter_Model.h>

// particles
#include <SEED/Source/Object/Particle/BaseParticle.h>
#include <SEED/Source/Object/Particle/Particle_Model.h>

class EffectSystem{

private:
    EffectSystem() = default;
    // コピー禁止
    EffectSystem(const EffectSystem&) = delete;
    void operator=(const EffectSystem&) = delete;

public:
    ~EffectSystem();
    static EffectSystem* GetInstance();
    static void Initialize();
    static void Update();
    static void Draw();

private:
    void LoadParticleTexture();

public:

    /// <summary>
    /// 加速フィールドを作成する
    /// </summary>
    /// <param name="range">フィールドの範囲</param>
    /// <param name="force">加速度</param>
    static void CreateAccelerationField(const Range3D& range, const Vector3& force);

    /// <summary>
    /// エフェクトを出現させる
    /// </summary>
    static void AddEffectOnce(const std::string& fileName, const Vector3& position, const Matrix4x4* parentMat);
    static void AddEffectOnce(const std::string& fileName, const Matrix4x4* parentMat);
    static void AddEffectOnce(const std::string& fileName, const Vector3& position);
    static uint32_t AddEffectEndless(const std::string& fileName, const Vector3& position, const Matrix4x4* parentMat);

    /// <summary>
    /// 削除
    /// </summary>
    static void DeleteAll();

private:

    /// <summary>
    /// エミッターの情報からパーティクルを発生させる
    /// </summary>
    static void Emit(Emitter_Base* emitter);

    /// <summary>
    /// ImGuiでエミッターの編集
    /// </summary>
    void Edit();

private:// ファイルの入出力

    // jsonファイルから読み込み
    void LoadFromJson(EmitterGroup* emitterGroup, const std::string& fileName);
    EmitterGroup LoadFromJson(const std::string& fileName);
    // ファイルを選択して読み込み
    void Load();

private:

    // パーティクルと加速フィールドの衝突判定
    void CollisionParticle2Field();

private:

    static EffectSystem* instance_;

private:

    bool isFieldActive_;
    bool isFieldVisible_ = true;

    // エフェクトデータ
    std::unordered_map<std::string, EmitterGroup> effectData_;// ロード済みのエフェクト情報
    std::list<std::unique_ptr<EmitterGroup>> onceEffects_;// 一度出現させて消えるエフェクトのリスト
    std::list<std::pair<uint32_t,std::unique_ptr<EmitterGroup>>> endlessEffects_;// 命令するまで消えないエフェクトのリスト

    // エミッター,パーティクル、フィールドのリスト
    std::list<std::unique_ptr<BaseParticle>> particles_;
    std::list<std::unique_ptr<AccelerationField>> accelerationFields_;

    // エディター用変数
    std::list<std::unique_ptr<EmitterGroup>> emitterGroups_;// エディターから追加されるエミッターグループ
};