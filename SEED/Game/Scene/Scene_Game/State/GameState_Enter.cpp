#include "GameState_Enter.h"

// others State
#include "GameState_Select.h"

//uiState
#include "../UI/State/UiState_LerpColor.h"

GameState_Enter::GameState_Enter(Scene_Base* _host)
    : State_Base(_host){
    Initialize();
}

GameState_Enter::~GameState_Enter(){}

void GameState_Enter::Initialize(){
    //============================== whiteScreen ==============================//
    fadeInScreen_ = std::make_unique<UI>("fadeInScreen");
    fadeInScreen_->Initialize("Assets/white1x1.png");
    fadeInScreen_->SetSize({1280.f,720.f});

    // state
    {
        std::unique_ptr<UiState_LerpColor> lerpColor = std::make_unique<UiState_LerpColor>(fadeInScreen_.get(),"GameScene_Enter");
        fadeInScreen_->SetState(std::move(lerpColor));
    }
}

void GameState_Enter::Update(){
    fadeInScreen_->Update();
    ManageState();
}

void GameState_Enter::Draw(){
    fadeInScreen_->Draw();
}

void GameState_Enter::Finalize(){
    fadeInScreen_->Finalize();
}

void GameState_Enter::BeginFrame(){
    fadeInScreen_->BeginFrame();
}

void GameState_Enter::EndFrame(){
    fadeInScreen_->EndFrame();
}

void GameState_Enter::HandOverColliders(){}

void GameState_Enter::ManageState(){
    if(fadeInScreen_->GetColor().w <= 0.f){
        pScene_->ChangeState(new GameState_Select(pScene_));
    }
}
