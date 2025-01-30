#include "TitleState_Out.h"

//parent
#include "Scene_Title.h"

// others State
#include "TitleState_Main.h"

/// lib
#include "ClockManager.h"

//math
#include "Easing.h"

TitleState_Out::TitleState_Out(Scene_Title* _host)
    : ITitleState(_host){
    Initialize();
}

TitleState_Out::~TitleState_Out(){}

void TitleState_Out::Initialize(){
    //============================== whiteScreen ==============================//
    whiteScreen_ = std::make_unique<Sprite>("Assets/white1x1.png");
    whiteScreen_->size = {1280.f,720.f};
}

void TitleState_Out::Update(){
    fadeTimer_ += ClockManager::DeltaTime();

    whiteScreen_->color.w = 1.0f - EaseInQuad(fadeTimer_ / fadeTime_);
}

void TitleState_Out::Draw(){}

void TitleState_Out::Finalize(){}

void TitleState_Out::BeginFrame(){}

void TitleState_Out::EndFrame(){

    ITitleState::EndFrame();
}

void TitleState_Out::HandOverColliders(){}

void TitleState_Out::ManageState(){
    if(fadeTimer_ >= fadeTime_){
        host_->ChangeState(new TitleState_Main(host_));
    }
}
