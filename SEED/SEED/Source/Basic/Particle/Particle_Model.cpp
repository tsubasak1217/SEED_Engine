#include "Particle_Model.h"
#include "Emitter/Emitter_Model.h"

Particle_Model::Particle_Model(Emitter_Base* emitter) : BaseParticle(emitter){

    Emitter_Model* modelEmitter = dynamic_cast<Emitter_Model*>(emitter);
    if(!modelEmitter){
        return;
    }

    ////////////////// パーティクルのモデルを生成 /////////////////////
    particle_ = std::make_unique<Model>(modelEmitter->emitModelFilePath_);
    particle_->isParticle_ = true;
    particle_->cullMode_ = modelEmitter->cullingMode; // カリングモードを設定

    /////////////////////// フラグ類を決定 ///////////////////////////

    isBillboard_ = modelEmitter->isBillboard;
    isUseGravity_ = modelEmitter->isUseGravity;
    isUseRotate_ = modelEmitter->isUseRotate;

    ////////////////////// 座標をランダム決定 ////////////////////////
    Range3D emitRange;
    Vector3 center = modelEmitter->GetCenter();
    emitRange.min = center - modelEmitter->emitRange * 0.5f;
    emitRange.max = center + modelEmitter->emitRange * 0.5f;
    particle_->transform_.translate = MyFunc::Random(emitRange);
    emitPos_ = particle_->transform_.translate;

    ///////////////////// 大きさをランダム決定 ////////////////////////
    float radius = MyFunc::Random(modelEmitter->radiusRange.min, modelEmitter->radiusRange.max);
    particle_->transform_.scale = MyFunc::Random(modelEmitter->scaleRange) * radius;
    kScale_ = particle_->transform_.scale;

    /////////////////////// 進行方向を決定 ////////////////////////

    direction_ = modelEmitter->baseDirection; // 基本方向を設定

    // 目標位置が設定されている場合、方向を目標位置に向ける
    if(modelEmitter->isSetGoalPosition){
        direction_ = MyMath::Normalize(modelEmitter->goalPosition - particle_->transform_.translate);
    }

    // 目標地点で終了するかどうかで処理を分岐
    if(!modelEmitter->isEndWithGoalPosition){
        // ばらけさせるやつ
        float angleRange = 3.14f * std::clamp(modelEmitter->directionRange, 0.0f, 1.0f);
        float theta = MyFunc::Random(-angleRange, angleRange); // 水平回転
        float phi = MyFunc::Random(-angleRange / 2.0f, angleRange / 2.0f); // 垂直回転 (範囲を制限)
        Vector3 randomDirection = {// 球座標から方向ベクトルを計算
            std::cos(phi) * std::cos(theta),
            std::cos(phi) * std::sin(theta),
            std::sin(phi)
        };

        // 基本方向とランダムな方向を組み合わせて最終的な方向を決定
        direction_ = randomDirection * Quaternion::DirectionToDirection({ 1.0f, 0.0f, 0.0f }, direction_); // 回転を適用
    
    } else{
        // 目標地点を設定
        goalPos_ = modelEmitter->GetCenter() + modelEmitter->goalPosition;
    }

    ///////////////////// 速度を決定 ////////////////////////
    if(!modelEmitter->isEndWithGoalPosition){
        speed_ = MyFunc::Random(modelEmitter->speedRange.min, modelEmitter->speedRange.max);
    }

    //////////////////////// 回転情報を決定 ////////////////////////

    // 回転軸指定がある場合、軸を決定
    if(modelEmitter->useRotateDirection){
        rotateAxis_ = modelEmitter->rotateDirection; // 基本方向を使用
    } else{
        rotateAxis_ = MyFunc::RandomVector();
    }

    // ランダム初期化フラグがある場合
    if(modelEmitter->isRoteteRandomInit_){
        particle_->transform_.rotate = Quaternion::ToQuaternion(rotateAxis_ * MyFunc::Random(-3.14f, 3.14f));
        localRotate_ = particle_->transform_.rotate;
    }

    // 回転速度の決定
    rotateSpeed_ = MyFunc::Random(modelEmitter->rotateSpeedRange.min, modelEmitter->rotateSpeedRange.max);

    //////////////////////// 重力を決定 //////////////////////////
    gravity_ = modelEmitter->gravity;

    ///////////////////// 寿命をランダム決定 ////////////////////////
    kLifeTime_ = MyFunc::Random(modelEmitter->lifeTimeRange.min, modelEmitter->lifeTimeRange.max);
    kLifeTime_ = std::clamp(kLifeTime_, 0.1f, 100000.0f); // 寿命の範囲を制限

    ////////////////////// 色をランダム決定 ////////////////////////
    particle_->masterColor_ = modelEmitter->colors[MyFunc::Random(0, (int)modelEmitter->colors.size() - 1)];

    ///////////////////// ブレンドモードを設定 ////////////////////////
    particle_->blendMode_ = modelEmitter->blendMode;

    //////////////////////// ライト設定 //////////////////////////
    particle_->lightingType_ = (int32_t)modelEmitter->lightingType_;

    //////////////////////// テクスチャを設定 ////////////////////////

    // モデルのテクスチャを取得し、無ければ追加
    if(modelEmitter->useDefaultTexture){
        ModelData* modelData = ModelManager::GetModelData(modelEmitter->emitModelFilePath_);
        auto& materials = modelData->materials;
        for(auto material : materials){
            std::string& texturePath = material.textureFilePath_;
            if(modelEmitter->textureSet.find(texturePath) != modelEmitter->textureSet.end()){
                continue; // 既に追加されているテクスチャはスキップ
            }
            modelEmitter->texturePaths.push_back(texturePath);
            modelEmitter->textureSet.insert(texturePath); // テクスチャセットに追加

            // GPUハンドルを取得し追加
            uint32_t handle = TextureManager::LoadTexture(texturePath);
            Emitter_Base::textureDict[texturePath] =
                (ImTextureID)ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, handle).ptr;
        }

    } else{// カスタムテクスチャを使用する場合、メッシュごとにランダムなテクスチャを設定

        for(auto& material : particle_->materials_){
            textureHandle_ = TextureManager::LoadTexture(
                modelEmitter->texturePaths[MyFunc::Random(0, (int)modelEmitter->texturePaths.size() - 1)]
            );
            material.GH = textureHandle_;
        }
    }

    //////////////////// イージング関数を設定 ////////////////////////
    velocityEaseFunc_ = Easing::Ease[modelEmitter->velocityEaseType_];
    rotateEaseFunc_ = Easing::Ease[modelEmitter->rotateEaseType_];
    enterEaseFunc_ = Easing::Ease[modelEmitter->enterEaseType_];
    exitEaseFunc_ = Easing::Ease[modelEmitter->exitEaseType_];

    //////////////////////// 減衰の設定 ////////////////////////
    kInScale_ = modelEmitter->kInScale;
    kOutScale_ = modelEmitter->kOutScale;
    kInAlpha_ = modelEmitter->kInAlpha;
    kOutAlpha_ = modelEmitter->kOutAlpha;
    maxTimePoint_ = modelEmitter->maxTimePoint;
    maxTimeRate_ = modelEmitter->maxTimeRate;
    // 出現の終了時間
    borderTime_[0] = (kLifeTime_ * maxTimePoint_) - (maxTimeRate_ * kLifeTime_ * 0.5f);
    borderTime_[0] = std::clamp(borderTime_[0], 0.0f, kLifeTime_);
    // 消失の開始時間
    borderTime_[1] = (kLifeTime_ * maxTimePoint_) + (maxTimeRate_ * kLifeTime_ * 0.5f);
    borderTime_[1] = std::clamp(borderTime_[1], 0.0f, kLifeTime_);
}

void Particle_Model::Update(){

    // 出現時・消失時の動き(内部で早期リターンしている) 
    Enter();
    Exit();

    // 寿命、ビルボードの更新
    BaseParticle::Update();
}
