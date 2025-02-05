#include "GameState_Out.h"

//parent
#include "Scene_Base.h"

// others Scene
#include "Scene_Clear.h"

/// lib
#include "ClockManager.h"

//math
#include "Easing.h"

GameState_Out::GameState_Out(Scene_Base* _host)
    : State_Base(_host){
    Initialize();
}

GameState_Out::~GameState_Out(){}

void GameState_Out::Initialize(){
    //============================== whiteScreen ==============================//
    whiteScreen_ = std::make_unique<Sprite>("Assets/white1x1.png");
    whiteScreen_->size = {1280.f,720.f};
}

void GameState_Out::Update(){
    fadeTimer_ += ClockManager::DeltaTime();

    whiteScreen_->color.w = EaseInQuad(fadeTimer_ / fadeTime_);

    currentBgmVolume_ = MyMath::Lerp(0.f,maxBGMVolume_,fadeTimer_ / fadeTime_);
    AudioManager::SetAudioVolume("BGM/Title.wav",currentBgmVolume_);
}

void GameState_Out::Draw(){
    whiteScreen_->Draw();
}

void GameState_Out::Finalize(){}

void GameState_Out::BeginFrame(){}

void GameState_Out::EndFrame(){
    ManageState();
}

void GameState_Out::HandOverColliders(){}

void GameState_Out::ManageState(){
    if(fadeTimer_ >= fadeTime_){
        pScene_->ChangeScene("Clear");
        AudioManager::EndAudio("BGM/title.wav");
    }
}
