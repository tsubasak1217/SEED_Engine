#include "ParticleManager.h"

ParticleManager* ParticleManager::instance_ = nullptr;


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

///////////////////////////////////////////////
// 更新処理
///////////////////////////////////////////////
void ParticleManager::Update(){

    // パーティクルの更新
    for(auto& particle : instance_->particles_){
        particle->Update();
    }

    // 死んでいるパーティクルを削除
    instance_->particles_.remove_if([](const std::shared_ptr<BaseParticle>& particle){
        return !particle->GetIsAlive();
    });
}

///////////////////////////////////////////////
// 描画処理
///////////////////////////////////////////////
void ParticleManager::Draw(){

    // パーティクルの描画
    for(auto& particle : instance_->particles_){
        particle->Draw();
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
    float lifeTime,
    const std::initializer_list<Vector4>& colors,
    int32_t count,
    BlendMode blendMode
){

    for(int32_t i = 0; i < count; ++i){

        switch(type){
        case ParticleType::kRadial:
            instance_->particles_.push_back(
                std::make_shared<RadialParticle>(
                    positionRange, radiusRange, speedRange, lifeTime, colors, blendMode
                ));

            break;
        }
    }
}


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
void ParticleManager::Emit(
    ParticleType type,
    const Range3D& positionRange,
    const Range1D& radiusRange,
    const Range1D& speedRange,
    float lifeTime,
    const std::initializer_list<Vector4>& colors,
    float interval, int32_t count, BlendMode blendMode
){
    static float timer = 0.0f;
    timer += ClockManager::DeltaTime();

    if(timer >= interval){
        Emit(type, positionRange, radiusRange, speedRange, lifeTime, colors, count, blendMode);
        timer = 0.0f;
    }
}


