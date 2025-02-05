#include "ClearState_Out.h"

//parent
#include "Scene_Base.h"

// others Scene
#include "Scene_Clear.h"

/// lib
#include "ClockManager.h"

//math
#include "Easing.h"

ClearState_Out::ClearState_Out(Scene_Base* _host)
    : State_Base(_host){
    Initialize();
}

ClearState_Out::~ClearState_Out(){}

void ClearState_Out::Initialize(){
    //============================== whiteScreen ==============================//
    whiteScreen_ = std::make_unique<Sprite>("Assets/white1x1.png");
    whiteScreen_->size = {1280.f,720.f};
}

void ClearState_Out::Update(){
    fadeTimer_ += ClockManager::DeltaTime();

    whiteScreen_->color.w = EaseInQuad(fadeTimer_ / fadeTime_);
}

void ClearState_Out::Draw(){
    whiteScreen_->Draw();
}

void ClearState_Out::Finalize(){}

void ClearState_Out::BeginFrame(){}

void ClearState_Out::EndFrame(){
    ManageState();
}

void ClearState_Out::HandOverColliders(){}

void ClearState_Out::ManageState(){
    if(fadeTimer_ >= fadeTime_){
        pScene_->ChangeScene("Game");
    }
}
