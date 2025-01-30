#include "GameState_Enter.h"

// others State
#include "GameState_Select.h"

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

    whiteScreen_->color.w = EaseInQuad(fadeTimer_ / fadeTime_);
    ManageState();
}

void GameState_Enter::Draw(){}

void GameState_Enter::Finalize(){}

void GameState_Enter::BeginFrame(){}

void GameState_Enter::EndFrame(){}

void GameState_Enter::HandOverColliders(){}

void GameState_Enter::ManageState(){
    if(fadeTimer_ >= fadeTime_){
        pScene_->ChangeState(new GameState_Select(pScene_));
    }
}
