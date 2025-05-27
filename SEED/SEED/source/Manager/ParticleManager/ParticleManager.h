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
#include <SEED/Source/Object/Particle/Emitter/Emitter_Plane.h>

// particles
#include <SEED/Source/Object/Particle/BaseParticle.h>
#include <SEED/Source/Object/Particle/Particle_Plane.h>

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
    static void AddEffect(const std::string& fileName, const Vector3& position, const Matrix4x4* parentMat);
    static void AddEffect(const std::string& fileName, const Matrix4x4* parentMat);
    static void AddEffect(const std::string& fileName, const Vector3& position);

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

    static ParticleManager* instance_;

private:

    bool isFieldActive_;
    bool isFieldVisible_ = true;

    // エフェクトデータ
    std::unordered_map<std::string, EmitterGroup> effectData_;// ロードしたエフェクトの情報
    std::list<std::unique_ptr<EmitterGroup>> effects_;// エフェクトのリスト

    // エミッター,パーティクル、フィールドのリスト
    std::list<std::unique_ptr<Emitter_Base>> emitters_;
    std::list<std::unique_ptr<BaseParticle>> particles_;
    std::list<std::unique_ptr<AccelerationField>> accelerationFields_;

    // エディター用変数
    std::list<std::unique_ptr<EmitterGroup>> emitterGroups_;// エディターから追加されるエミッターグループ
};