#include "TitleState_Enter.h"

//parent
#include "Scene_Title.h"

// others State
#include "TitleState_Main.h"

//uiState
#include "../UI/State/UiState_LerpColor.h"

TitleState_Enter::TitleState_Enter(Scene_Title* _host)
    : ITitleState(_host){}

TitleState_Enter::~TitleState_Enter(){}

void TitleState_Enter::Initialize(){
    //============================== whiteScreen ==============================//
    whiteScreen_ = std::make_unique<UI>("whiteScreen");
    whiteScreen_->Initialize("Assets/white1x1.png");
    whiteScreen_->SetSize({1280.f,720.f});

    // state
    {
        std::unique_ptr<UiState_LerpColor> lerpColor = std::make_unique<UiState_LerpColor>(whiteScreen_.get(),"TitleEnter");
        whiteScreen_->SetState(std::move(lerpColor));
    }
}

void TitleState_Enter::Update(){
    whiteScreen_->Update();
}

void TitleState_Enter::Draw(){
    whiteScreen_->Draw();
}

void TitleState_Enter::Finalize(){
    whiteScreen_->Finalize();
}

void TitleState_Enter::BeginFrame(){
    whiteScreen_->BeginFrame();
}

void TitleState_Enter::EndFrame(){
    whiteScreen_->EndFrame();
}

void TitleState_Enter::HandOverColliders(){}

void TitleState_Enter::ManageState(){
    if(whiteScreen_->GetColor().w <= 0.f){
        host_->ChangeState(new TitleState_Main(host_));
    }
}
