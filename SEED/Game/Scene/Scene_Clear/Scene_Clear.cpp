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
    CameraManager::GetActiveCamera()->SetTranslation({0.f,0.f,-10.f});
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
    dinosaur_->StartAnimation("breakEgg",false);

    // transform Initialize
    dinosaur_->rotate_.y = 3.141592f;
    dinosaur_->translate_ = {0.0f,-1.2f,10.0f};

    dinosaur_->isRotateWithQuaternion_ = false;
    dinosaur_->isParentScale_ = false;
    dinosaur_->UpdateMatrix();

    // 先に読み込んでおく
    ModelManager::LoadModel("clear_dance.gltf");

    //=========================== eggTop =======================//
    eggTop_ = std::make_unique<Model>("eggTop_breakEgg.gltf");
    eggTop_->StartAnimation("breakEgg",false);
    // transform Initialize
    eggTop_->rotate_.y = 3.141592f;
    eggTop_->translate_ = {0.0f,-1.2f,10.0f};

    eggTop_->isRotateWithQuaternion_ = false;
    eggTop_->isParentScale_ = false;
    eggTop_->UpdateMatrix();

    // 先に読み込んでおく
    ModelManager::LoadModel("eggTop_dance.gltf");

    //=========================== eggBottom =======================//
    eggBottom_ = std::make_unique<Model>("eggBottom_breakEgg.gltf");
    eggBottom_->StartAnimation("breakEgg",false);
    // transform Initialize
    eggBottom_->rotate_.y = 3.141592f;
    eggBottom_->translate_ = {0.0f,-1.2f,10.0f};

    eggBottom_->isRotateWithQuaternion_ = false;
    eggBottom_->isParentScale_ = false;
    eggBottom_->UpdateMatrix();

    // 先に読み込んでおく
    ModelManager::LoadModel("eggBottom_dance.gltf");

    //=========================== corpseEmitter =======================//
    corpseEmitter_ = std::make_unique<CorpseEmitter>();
    for(auto& corpseModel : corpseEmitter_->particles_){
        corpseModel  = std::make_unique<Model>();
        corpseModel->Initialize("dinosaur_corpse.obj");
        corpseModel->isRotateWithQuaternion_ = false;
        corpseModel->color_ = MyMath::FloatColor(0x030ff30ff);
    }

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
    ImGui::Begin("dinosaur");
    ImGui::DragFloat3("scale",&dinosaur_->scale_.x,0.01f);
    ImGui::DragFloat3("rotate",&dinosaur_->rotate_.x,0.01f);
    ImGui::DragFloat3("translate",&dinosaur_->translate_.x,0.01f);
    ImGui::End();

    UpdateCorpseParticles();

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
    ImGui::Begin("CorpseEmitter");
    ImGui::DragFloat3("min",&corpseEmitter_->min_.x,0.01f);
    ImGui::DragFloat3("max",&corpseEmitter_->max_.x,0.01f);

    ImGui::DragFloat("EmitCoolTime",&corpseEmitter_->emitCoolTime_,0.01f);
    ImGui::DragInt("EmitValueMin",&corpseEmitter_->emitValueMin_);
    ImGui::DragInt("EmitValueMax",&corpseEmitter_->emitValueMax_);
    ImGui::End();

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