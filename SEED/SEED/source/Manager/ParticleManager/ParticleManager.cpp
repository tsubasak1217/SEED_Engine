#include "ParticleManager.h"
#include "ImGuiManager.h"

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

/// <summary>
/// 
/// </summary>
ParticleManager::~ParticleManager(){
    delete instance_;
    instance_ = nullptr;
}


/// <summary>
/// 
/// </summary>
/// <returns></returns>
ParticleManager* ParticleManager::GetInstance(){
    if(!instance_){
        instance_ = new ParticleManager();
    }
    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                     更新処理                                                   */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Update(){


#ifdef _DEBUG
    ImGui::Begin("isFieldActive");
    ImGui::Checkbox("isFieldActive", &instance_->isFieldActive_);
    ImGui::End();
#endif // _DEBUG

    ////////////////////////////////////////////
    // エミッターの更新
    ////////////////////////////////////////////
    for(auto& emitter : instance_->emitters_){
        emitter->Update();

        // 発生命令が出ていればパーティクルを発生させる
        if(emitter->emitOrder == true){
            Emit(*emitter);
        }
    }
    
    ////////////////////////////////////////////
    // パーティクルの更新
    ////////////////////////////////////////////
    for(auto& particle : instance_->particles_){
        particle->Update();
    }

    ////////////////////////////////////////////
    // パーティクルとフィールドの衝突判定
    ////////////////////////////////////////////
    if(instance_->isFieldActive_){
        instance_->CollisionParticle2Field();
    }

    ////////////////////////////////////////////
    // 死んでいる要素の削除
    ////////////////////////////////////////////
    instance_->particles_.remove_if([](auto& particle){
        return !particle->GetIsAlive();
    });

    instance_->emitters_.remove_if([](auto& emitter){
        return !emitter->isActive;
    });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                   描画関数                                                      */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Draw(){

    // パーティクルの描画
    for(auto& particle : instance_->particles_){
        particle->Draw();
    }

#ifdef _DEBUG
    for(auto& AccelerationField : instance_->accelerationFields_){
        AccelerationField->DrawRange();
    }
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                              領域を追加する関数                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 加速フィールドを作成する
/// </summary>
/// <param name="range">フィールドの範囲</param>
/// <param name="force">加速度</param>
void ParticleManager::CreateAccelerationField(const Range3D& range, const Vector3& force){
    instance_->accelerationFields_.emplace_back(
        std::make_unique<AccelerationField>(force, range)
    );
}

/// <summary>
/// エミッターを追加する
/// </summary>
/// <param name="emitter"></param>
void ParticleManager::AddEmitter(const Emitter& emitter){
    instance_->emitters_.emplace_back(std::make_unique<Emitter>(emitter));
}


/// <summary>
/// パーティクルを発生させる(直接)
/// </summary>
/// <param name="emitType"></param>
/// <param name="particleType"></param>
/// <param name="positionRange"></param>
/// <param name="radiusRange"></param>
/// <param name="speedRange"></param>
/// <param name="lifeTimeRange"></param>
/// <param name="colors"></param>
/// <param name="interval"></param>
/// <param name="numEmitEvery"></param>
/// <param name="blendMode"></param>
/// <param name="numEmitMax"></param>
void ParticleManager::AddEmitter(
    EmitType emitType, ParticleType particleType,
    const Range3D& positionRange, const Range1D& radiusRange,
    const Range1D& speedRange, const Range1D& lifeTimeRange, 
    const std::initializer_list<Vector4>& colors, 
    float interval, int32_t numEmitEvery, BlendMode blendMode, int32_t numEmitMax
){
    Emitter emitter;
    emitter.emitType = emitType;
    emitter.particleType = particleType;
    emitter.positionRange = positionRange;
    emitter.radiusRange = radiusRange;
    emitter.speedRange = speedRange;
    emitter.lifeTimeRange = lifeTimeRange;
    emitter.colors = colors;
    emitter.interval = interval;
    emitter.numEmitEvery = numEmitEvery;
    emitter.blendMode = blendMode;
    emitter.kMaxEmitCount = numEmitMax;

    instance_->emitters_.emplace_back(std::make_unique<Emitter>(emitter));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                          パーティクルを追加する関数                                               */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// パーティクルを発生させる
/// </summary>
/// <param name="emitter"></param>
void ParticleManager::Emit(Emitter& emitter){

    if(emitter.emitOrder){

        Emit(
            emitter.particleType,
            emitter.positionRange,
            emitter.radiusRange,
            emitter.speedRange,
            emitter.lifeTimeRange,
            emitter.colors,
            emitter.numEmitEvery,
            emitter.blendMode
        );

        emitter.emitOrder = false;
    }
}

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
void ParticleManager::Emit(
    ParticleType type,
    const Range3D& positionRange,
    const Range1D& radiusRange,
    const Range1D& speedRange,
    const Range1D& lifeTimeRange,
    const std::vector<Vector4>& colors,
    int32_t numEmit,
    BlendMode blendMode
){

    for(int32_t i = 0; i < numEmit; ++i){

        switch(type){
        case ParticleType::kRadial:
            instance_->particles_.emplace_back(
                std::make_unique<RadialParticle>(
                    positionRange, radiusRange, speedRange, lifeTimeRange, colors, blendMode
                ));

            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                パーティクルと加速フィールドの当たり判定関数                                          */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// パーティクルと加速フィールドの衝突判定
/// </summary>
void ParticleManager::CollisionParticle2Field(){
    for(auto& particle : instance_->particles_){
        for(auto& field : instance_->accelerationFields_){
            if(field->CheckCollision(particle->GetPos())){
                particle->SetAcceleration(field->acceleration);
            }
        }
    }
}


