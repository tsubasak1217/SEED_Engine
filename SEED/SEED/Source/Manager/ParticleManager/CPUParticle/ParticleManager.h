#pragma once
// stl
#include <list>
#include <memory>
#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <json.hpp>
// SEED
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/AccelerarionField.h>

// emitters
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroupBase.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/Emitter3D.h>

namespace SEED{
    // particles
    class Particle3D;
    class Particle2D;

    //////////////////////////////////////////////////////////////////////////////////
    // エミッターが生成したパーティクルや加速フィールドを管理するクラス
    //////////////////////////////////////////////////////////////////////////////////
    class ParticleManager{

    private:
        ParticleManager() = default;
        // コピー禁止
        ParticleManager(const ParticleManager&) = delete;
        void operator=(const ParticleManager&) = delete;
        static ParticleManager* instance_;

    public:
        ~ParticleManager();
        static ParticleManager* GetInstance();
        static void Initialize();
        static void Update();
        static void Draw();

    public:
        // パーティクルを発生させる
        static void Emit(EmitterBase* emitter);
        // 加速フィールドを作成する
        static void CreateAccelerationField(const Range3D& range, const Vector3& force);
        // 削除
        static void DeleteAll();

    private:
        // パーティクルと加速フィールドの衝突判定
        void CollisionParticle2Field();

    private:

        bool isFieldActive_;
        bool isFieldVisible_ = true;

        // パーティクル、フィールドのリスト
        std::list<std::unique_ptr<Particle3D>> particles3D_;
        std::list<std::unique_ptr<Particle2D>> particles2D_;
        std::list<std::unique_ptr<AccelerationField>> accelerationFields_;

        // 読み込み済みのグループ情報のjsonのマップ
        std::unordered_map<std::string, nlohmann::json> loadedEffects_;
    };
}