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
    pDinoModel_ = pClearScene_->GetDinosaur();
}

void ClearState_Main::Update(){
    pDinoModel_->Update();
    if(!isBreakEgg_){
        if(pDinoModel_->GetIsAnimation()){
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
    std::unique_ptr<Model> dinoModel = std::make_unique<Model>("clear_dance.gltf");
    dinoModel->StartAnimation("dance",true);

    // transform Initialize
    dinoModel->rotate_.y = 3.141592f;
    dinoModel->translate_ = {0.0f,-1.2f,10.0f};

    dinoModel->isRotateWithQuaternion_ = false;
    dinoModel->isParentScale_ = false;
    dinoModel->UpdateMatrix();
    pClearScene_->SetDinosaur(std::move(dinoModel));

    pDinoModel_ = pClearScene_->GetDinosaur();
}
