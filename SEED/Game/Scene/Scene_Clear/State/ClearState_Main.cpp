#include "ClearState_Main.h"

#include "Scene_Clear.h"

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

void ClearState_Main::EndFrame(){}

void ClearState_Main::HandOverColliders(){}

void ClearState_Main::ManageState(){}

void ClearState_Main::ChangeModel(){
    pDinoModel_->Initialize("clear_dance.gltf");
    pDinoModel_->StartAnimation("dance",true);
    // transform Initialize
    pDinoModel_->rotate_.y = 3.141592f;
    pDinoModel_->translate_ = {0.0f,-1.2f,10.0f};

    pDinoModel_->isRotateWithQuaternion_ = false;
    pDinoModel_->isParentScale_ = false;
    pDinoModel_->UpdateMatrix();
}
