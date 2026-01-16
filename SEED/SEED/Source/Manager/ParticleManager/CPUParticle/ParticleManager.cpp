#include "ParticleManager.h"
#include <execution>
#include <algorithm>

#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroupBase.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/Emitter3D.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/Emitter2D.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Particle/Particle3D.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Particle/Particle2D.h>

namespace SEED{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*                                                                                                               */
    /*                                            statich変数の初期化                                                  */
    /*                                                                                                               */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ParticleManager* ParticleManager::instance_ = nullptr;


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*                                                                                                               */
    /*                                                初期化・終了関数                                                  */
    /*                                                                                                               */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// デストラクタ
    ParticleManager::~ParticleManager(){
        delete instance_;
        instance_ = nullptr;
    }


    /// インスタンスを取得
    ParticleManager* ParticleManager::GetInstance(){
        if(!instance_){
            instance_ = new ParticleManager();
        }
        return instance_;
    }

    /// 初期化
    void ParticleManager::Initialize(){
        // インスタンスを取得
        GetInstance();
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 更新処理
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ParticleManager::Update(){

        //==============================================//
        // パーティクルの更新
        //==============================================//
        std::for_each(std::execution::par_unseq, instance_->particles3D_.begin(), instance_->particles3D_.end(), [](auto& p){
            p->Update();
        });
        std::for_each(std::execution::par_unseq, instance_->particles2D_.begin(), instance_->particles2D_.end(), [](auto& p){
            p->Update();
        });

        //==============================================//
        // パーティクルとフィールドの衝突判定
        //==============================================//
        if(instance_->isFieldActive_){
            instance_->CollisionParticle2Field();
        }

        //==============================================//
        // 死んでいる要素の削除
        //==============================================//
        instance_->particles3D_.remove_if([](auto& particle){
            return !particle->GetIsAlive();
        });
        instance_->particles2D_.remove_if([](auto& particle){
            return !particle->GetIsAlive();
        });
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 描画処理
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ParticleManager::Draw(){

        // パーティクルの描画
        for(auto& particle : instance_->particles3D_){
            particle->Draw();
        }

        for(auto& particle : instance_->particles2D_){
            particle->Draw();
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 加速フィールドを作成する
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ParticleManager::CreateAccelerationField(const Range3D& range, const Vector3& force){
        instance_->accelerationFields_.emplace_back(
            std::make_unique<AccelerationField>(force, range)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // エフェクトを削除する
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ParticleManager::DeleteAll(){
        instance_->particles3D_.clear();
        instance_->particles2D_.clear();
        instance_->accelerationFields_.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // パーティクルを発生させる
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ParticleManager::Emit(EmitterBase* emitter){

        if(!emitter->emitOrder_){ return; }
        if(!emitter->isActive_){ return; }

        // パーティクルを発生させる
        if(Emitter3D* emitter3D = dynamic_cast<Emitter3D*>(emitter)){
            for(int32_t i = 0; i < emitter->numEmitEvery_; ++i){
                instance_->particles3D_.emplace_back(
                    std::make_unique<Particle3D>(emitter3D)
                );
            }

        } else if(Emitter2D* emitter2D = dynamic_cast<Emitter2D*>(emitter)){
            for(int32_t i = 0; i < emitter->numEmitEvery_; ++i){
                instance_->particles2D_.emplace_back(
                    std::make_unique<Particle2D>(emitter2D)
                );
            }
        }

        // 発生命令をリセット
        emitter->emitOrder_ = false;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // パーティクルと加速フィールドの当たり判定関数
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// <summary>
    /// パーティクルと加速フィールドの衝突判定
    /// </summary>
    void ParticleManager::CollisionParticle2Field(){
        for(auto& particle : instance_->particles3D_){
            for(auto& field : instance_->accelerationFields_){
                if(field->CheckCollision(particle->GetPos())){
                    particle->SetAcceleration(field->acceleration);
                }
            }
        }

        // 2D版は未実装
    }
}