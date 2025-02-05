#include "GameState_Enter.h"

// others State
#include "GameState_Title.h"

// math
#include "Easing.h"

GameState_Enter::GameState_Enter(Scene_Base* _host)
    : State_Base(_host){
    Initialize();
}

GameState_Enter::~GameState_Enter(){}

void GameState_Enter::Initialize(){
    //============================== whiteScreen ==============================//
    whiteScreen_ = std::make_unique<Sprite>("Assets/white1x1.png");
    whiteScreen_->size = {1280.f,720.f};
}

void GameState_Enter::Update(){
    fadeTimer_ += ClockManager::DeltaTime();

    whiteScreen_->color.w = 1.f - EaseInQuad(fadeTimer_ / fadeTime_);
    currentBgmVolume_ = MyMath::Lerp(0.f,maxBGMVolume_,fadeTimer_ / fadeTime_);
    AudioManager::SetAudioVolume("BGM/title.wav",currentBgmVolume_);
}

void GameState_Enter::Draw(){
    whiteScreen_->Draw();
}

void GameState_Enter::Finalize(){}

void GameState_Enter::BeginFrame(){}

void GameState_Enter::EndFrame(){
    ManageState();
}

void GameState_Enter::HandOverColliders(){}

void GameState_Enter::ManageState(){
    if(fadeTimer_ >= fadeTime_){
        pScene_->ChangeState(new GameState_Title(pScene_));
    }
}
