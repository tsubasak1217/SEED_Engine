#include "ClearState_Enter.h"

//hostObject
#include "Scene_Base.h"
#include "Scene_Clear.h"

// others State
#include "ClearState_Main.h"

//lib
#include "ClockManager.h"

// math
#include "Easing.h"

ClearState_Enter::ClearState_Enter(Scene_Base* _host)
    : State_Base(_host){
    Initialize();
}

ClearState_Enter::~ClearState_Enter(){}

void ClearState_Enter::Initialize(){
    //============================== whiteScreen ==============================//
    whiteScreen_ = std::make_unique<Sprite>("Assets/white1x1.png");
    whiteScreen_->size = {1280.f,720.f};
}

void ClearState_Enter::Update(){
    fadeTimer_ += ClockManager::DeltaTime();

    whiteScreen_->color.w = 1.f - EaseInQuad(fadeTimer_ / fadeTime_);
    currentBgmVolume_ = MyMath::Lerp(0.f,maxBGMVolume_,fadeTimer_ / fadeTime_);
    AudioManager::SetAudioVolume("BGM/clear.wav",currentBgmVolume_);
}

void ClearState_Enter::Draw(){
    whiteScreen_->Draw();
}

void ClearState_Enter::Finalize(){}

void ClearState_Enter::BeginFrame(){}

void ClearState_Enter::EndFrame(){
    ManageState();
}

void ClearState_Enter::HandOverColliders(){}

void ClearState_Enter::ManageState(){
    if(fadeTimer_ >= fadeTime_){
        pScene_->ChangeState(new ClearState_Main(dynamic_cast<Scene_Clear*>(pScene_)));
    }
}
