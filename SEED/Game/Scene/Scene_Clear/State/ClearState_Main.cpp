#include "ClearState_Main.h"

//host
#include "Scene_Clear.h"
// ohter stae
#include "ClearState_Out.h"
//lib
#include "../PlayerInput/PlayerInput.h"

ClearState_Main::ClearState_Main(Scene_Clear* scene)
    :State_Base(scene),
    pClearScene_(scene){
    Initialize();
}

ClearState_Main::~ClearState_Main(){}

void ClearState_Main::Initialize(){
    pClearScene_->GetCorpsesPile()->StartAnimation("breakEgg",false);
    pClearScene_->GetEggTop()->StartAnimation("eggTop",false);
    pClearScene_->GetEggBottom()->StartAnimation("eggBottom",false);
}

void ClearState_Main::Update(){
    if(!isBreakEgg_){
        if(pClearScene_->GetDinosaur()->GetIsEndAnimation()){
            isBreakEgg_ = true;
            ChangeModel();
        }
    }
}

void ClearState_Main::Draw(){}

void ClearState_Main::Finalize(){}

void ClearState_Main::BeginFrame(){}

void ClearState_Main::EndFrame(){
    ManageState();
}

void ClearState_Main::HandOverColliders(){}

void ClearState_Main::ManageState(){
    if(isBreakEgg_ && PlayerInput::TitleScene::IsStartGame()){
        pClearScene_->ChangeState(new ClearState_Out(pClearScene_));
    }
}

void ClearState_Main::ChangeModel(){
    // モデル作り直し

    //=========================== dinosaur =======================//
    {
        std::unique_ptr<Model> dinoModel = std::make_unique<Model>("clear_dance.gltf");
        dinoModel->StartAnimation("dance",true);

        // transform Initialize
        dinoModel->rotate_.y = 3.141592f;
        dinoModel->translate_ = {4.56f,-1.910f,10.0f};

        dinoModel->isRotateWithQuaternion_ = false;
        dinoModel->isParentScale_ = false;
        dinoModel->UpdateMatrix();
        pClearScene_->SetDinosaur(std::move(dinoModel));
    }

    //=========================== eggTop  =======================//
    {
        std::unique_ptr<Model> eggTopModel = std::make_unique<Model>("eggTop_dance.gltf");
        eggTopModel->StartAnimation("danceTop",true);
        // transform Initialize
        eggTopModel->rotate_.y = 3.141592f;
        eggTopModel->translate_ = {4.56f,-1.910f,10.0f};

        eggTopModel->isRotateWithQuaternion_ = false;
        eggTopModel->isParentScale_ = false;
        eggTopModel->UpdateMatrix();
        pClearScene_->SetEggTop(std::move(eggTopModel));
    }

    //=========================== eggBottom  =======================//
    {
        std::unique_ptr<Model> eggBottomModel = std::make_unique<Model>("eggBottom_dance.gltf");
        eggBottomModel->StartAnimation("danceButtom",true);
        // transform Initialize
        eggBottomModel->rotate_.y = 3.141592f;
        eggBottomModel->translate_ = {4.56f,-1.910f,10.0f};

        eggBottomModel->isRotateWithQuaternion_ = false;
        eggBottomModel->isParentScale_ = false;
        eggBottomModel->UpdateMatrix();
        pClearScene_->SetEggTop(std::move(eggBottomModel));
    }
}
