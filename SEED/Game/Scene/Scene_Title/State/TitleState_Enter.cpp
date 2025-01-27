#include "TitleState_Enter.h"

//parent
#include "Scene_Title.h"

// others State
#include "TitleState_Main.h"

//uiState
#include "../UI/State/UiState_LerpColor.h"

TitleState_Enter::TitleState_Enter(Scene_Title* _host)
    : ITitleState(_host){
    Initialize();
}

TitleState_Enter::~TitleState_Enter(){}

void TitleState_Enter::Initialize(){
    //============================== whiteScreen ==============================//
    fadeInScreen_ = std::make_unique<UI>("fadeInScreen");
    fadeInScreen_->Initialize("Assets/white1x1.png");
    fadeInScreen_->SetSize({1280.f,720.f});

    // state
    {
        std::unique_ptr<UiState_LerpColor> lerpColor = std::make_unique<UiState_LerpColor>(fadeInScreen_.get(),"TitleEnter");
        fadeInScreen_->SetState(std::move(lerpColor));
    }
}

void TitleState_Enter::Update(){
    fadeInScreen_->Update();
}

void TitleState_Enter::Draw(){
    fadeInScreen_->Draw();
}

void TitleState_Enter::Finalize(){
    fadeInScreen_->Finalize();
}

void TitleState_Enter::BeginFrame(){
    fadeInScreen_->BeginFrame();
}

void TitleState_Enter::EndFrame(){
    fadeInScreen_->EndFrame();

    ITitleState::EndFrame();
}

void TitleState_Enter::HandOverColliders(){}

void TitleState_Enter::ManageState(){
    if(fadeInScreen_->GetColor().w <= 0.f){
        host_->ChangeState(new TitleState_Main(host_));
    }
}
