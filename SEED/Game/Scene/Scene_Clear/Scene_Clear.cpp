#include "Scene_Clear.h"

//states
#include "State/ClearState_Enter.h"

///etc
#include "MyFunc.h"

////////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ・デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////////
Scene_Clear::Scene_Clear(){
    SEED::SetCamera("main");
    CameraManager::GetActiveCamera()->SetTranslation({2.f,3.f,-10.f});
    CameraManager::GetActiveCamera()->SetRotation({0.f,0.1f,0.f});
    Initialize();
}

Scene_Clear::~Scene_Clear(){}


////////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化
//
////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Clear::Initialize(){
    //=========================== dinosaur =======================//
    dinosaur_ = std::make_unique<Model>("clear_breakEgg.gltf");

    // transform Initialize
    dinosaur_->rotate_.y = 3.141592f;
    dinosaur_->translate_ = {4.56f,-1.910f,10.0f};

    dinosaur_->isRotateWithQuaternion_ = false;
    dinosaur_->isParentScale_ = false;
    dinosaur_->UpdateMatrix();

    // 先に読み込んでおく
    ModelManager::LoadModel("clear_dance.gltf");

    //=========================== eggTop =======================//
    eggTop_ = std::make_unique<Model>("eggTop_breakEgg.gltf");
    // transform Initialize
    eggTop_->rotate_.y = 3.141592f;
    eggTop_->translate_ = {4.56f,-1.910f,10.0f};

    eggTop_->isRotateWithQuaternion_ = false;
    eggTop_->isParentScale_ = false;
    eggTop_->UpdateMatrix();

    // 先に読み込んでおく
    ModelManager::LoadModel("eggTop_dance.gltf");

    //=========================== eggBottom =======================//
    eggBottom_ = std::make_unique<Model>("eggBottom_breakEgg.gltf");
    // transform Initialize
    eggBottom_->rotate_.y = 3.141592f;
    eggBottom_->translate_ = {4.56f,-1.910f,10.0f};

    eggBottom_->isRotateWithQuaternion_ = false;
    eggBottom_->isParentScale_ = false;
    eggBottom_->UpdateMatrix();

    // 先に読み込んでおく
    ModelManager::LoadModel("eggBottom_dance.gltf");

    //=========================== corpsePile =======================//
    corpsesPile_ = std::make_unique<Model>("corpsesPile.obj");
    corpsesPile_->isRotateWithQuaternion_ = false;
    corpsesPile_->translate_ = {4.36f,-5.41f,9.12f};

    //=========================== corpseEmitter =======================//
    corpseEmitter_ = std::make_unique<CorpseEmitter>();
    for(auto& corpseModel : corpseEmitter_->particles_){
        corpseModel  = std::make_unique<Model>();
        corpseModel->Initialize("dinosaur_corpse.obj");
        corpseModel->isRotateWithQuaternion_ = false;
        //corpseModel->color_ = MyMath::FloatColor(0x030ff30ff);
    }

    //=========================== clearText =======================//
    clearText_ = std::make_unique<Sprite>("ClearText/ClearText.png");

    //=========================== light =========================//
    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->color_ = MyMath::FloatColor(0xffffffff);
    directionalLight_->direction_ = MyMath::Normalize({1.0f,-1.0f,0.5f});
    directionalLight_->intensity = 1.0f;

    //=========================== state =========================//
    currentState_ = std::make_unique<ClearState_Enter>(this);
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Clear::Finalize(){}

////////////////////////////////////////////////////////////////////////////////////////////
//
// 更新
//
////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Clear::Update(){

    UpdateCorpseParticles();

    //=========================== Object =======================//
    dinosaur_->Update();
    eggTop_->Update();
    eggBottom_->Update();

    corpsesPile_->Update();

    //=========================== Text =======================//
    sinAnimationTime_ += ClockManager::DeltaTime();
    sinAnimationTime_ = fmod(sinAnimationTime_,3.141592f * 2.f);
    clearText_->color.w = sinf(sinAnimationTime_);

    //=========================== State =======================//
    if(currentState_){
        currentState_->Update();
    }

    //=========================== light =======================//
    directionalLight_->SendData();

}

////////////////////////////////////////////////////////////////////////////////////////////
//
// 描画
//
////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Clear::Draw(){
    //=========================== Object =======================//
    dinosaur_->Draw();
    eggTop_->Draw();
    eggBottom_->Draw();

    corpsesPile_->Draw();

    clearText_->Draw();

    for(int i = 0; i < corpseEmitter_->particles_.size(); i++){
        if(!corpseEmitter_->particleActiveStatus_[i]){
            continue;
        }
        corpseEmitter_->particles_[i]->Draw();
    }

    //=========================== State =======================//
    if(currentState_){
        currentState_->Draw();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Clear::BeginFrame(){
    Scene_Base::BeginFrame();

    if(currentState_){
        currentState_->BeginFrame();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Clear::EndFrame(){
    if(currentState_){
        currentState_->EndFrame();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Clear::HandOverColliders(){}

void Scene_Clear::UpdateCorpseParticles(){

    // spawn
    if(corpseEmitter_->leftCoolTime_ <= 0.f){
        corpseEmitter_->leftCoolTime_ = corpseEmitter_->emitCoolTime_;

        int emitValue = MyFunc::Random(corpseEmitter_->emitValueMin_,corpseEmitter_->emitValueMax_);
        for(int i = 0; i < corpseEmitter_->particles_.size(); i++){
            if(emitValue <= 0){
                break;
            }
            if(corpseEmitter_->particleActiveStatus_[i]){
                continue;
            }
            --emitValue;

            corpseEmitter_->particleActiveStatus_[i] = true;

            corpseEmitter_->particleDropSpeed_[i] = 0.f;

            float randomScale =  MyFunc::Random(0.5f,1.3f);
            corpseEmitter_->particles_[i]->scale_ = {randomScale,randomScale,randomScale};
            corpseEmitter_->particles_[i]->rotate_ = {MyFunc::Random(0.f,3.141592f),MyFunc::Random(0.f,3.141592f),MyFunc::Random(0.f,3.141592f)};
            corpseEmitter_->particles_[i]->translate_ = MyFunc::Random(corpseEmitter_->min_,corpseEmitter_->max_);

            corpseEmitter_->particles_[i]->UpdateMatrix();
        }
    }

    corpseEmitter_->leftCoolTime_ -= ClockManager::DeltaTime();

    // update
    for(int i = 0; i < corpseEmitter_->particles_.size(); i++){
        if(!corpseEmitter_->particleActiveStatus_[i]){
            continue;
        }
        corpseEmitter_->particleDropSpeed_[i] += 9.8f * 3.1f/*重さ分*/ * ClockManager::DeltaTime();

        corpseEmitter_->particles_[i]->translate_.y -= corpseEmitter_->particleDropSpeed_[i] * ClockManager::DeltaTime();
        corpseEmitter_->particles_[i]->UpdateMatrix();

        if(corpseEmitter_->particles_[i]->translate_.y <= -20.f){
            corpseEmitter_->particleActiveStatus_[i] = false;
        }
    }
}