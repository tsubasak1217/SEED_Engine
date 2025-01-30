#include "TitleState_Enter.h"

//parent
#include "Scene_Title.h"

// others State
#include "TitleState_Main.h"

/// lib
#include "ClockManager.h"

//math
#include "Easing.h"

TitleState_Enter::TitleState_Enter(Scene_Title* _host)
    : ITitleState(_host){
    Initialize();
}

TitleState_Enter::~TitleState_Enter(){}

void TitleState_Enter::Initialize(){
    //============================== whiteScreen ==============================//
    whiteScreen_ = std::make_unique<Sprite>("Assets/white1x1.png");
    whiteScreen_->size = {1280.f,720.f};
}

void TitleState_Enter::Update(){
    fadeTimer_ += ClockManager::DeltaTime();

    whiteScreen_->color.w = EaseInQuad(fadeTimer_ / fadeTime_);
}

void TitleState_Enter::Draw(){}

void TitleState_Enter::Finalize(){}

void TitleState_Enter::BeginFrame(){}

void TitleState_Enter::EndFrame(){

    ITitleState::EndFrame();
}

void TitleState_Enter::HandOverColliders(){}

void TitleState_Enter::ManageState(){
    if(fadeTimer_ >= fadeTime_){
        host_->ChangeState(new TitleState_Main(host_));
    }
}
