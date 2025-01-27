#include "TitleState_Out.h"

//parent
#include "Scene_Title.h"

//nextScene
#include "Scene_Game/Scene_Game.h"

//ui
#include "../UI/UI.h"
//ui state
#include "../UI/State/UiState_LerpColor.h"

TitleState_Out::TitleState_Out(Scene_Title* _host)
    : ITitleState(_host){
    Initialize();
}

TitleState_Out::~TitleState_Out(){}

void TitleState_Out::Initialize(){
    //============================== whiteScreen ==============================//
    fadeOutScreen_ = std::make_unique<UI>("fadeOutScreen");
    fadeOutScreen_->Initialize("Assets/white1x1.png");
    fadeOutScreen_->SetSize({1280.f,720.f});
    fadeOutScreen_->SetColor({0.f,0.f,0.f,0.f});
    // state
    {
        std::unique_ptr<UiState_LerpColor> lerpColor = std::make_unique<UiState_LerpColor>(fadeOutScreen_.get(),"TitleOut");
        fadeOutScreen_->SetState(std::move(lerpColor));
    }
}

void TitleState_Out::Update(){
    fadeOutScreen_->Update();
}

void TitleState_Out::Draw(){
    fadeOutScreen_->Draw();
}

void TitleState_Out::Finalize(){
    fadeOutScreen_->Finalize();
}

void TitleState_Out::BeginFrame(){
    fadeOutScreen_->BeginFrame();
}

void TitleState_Out::EndFrame(){
    fadeOutScreen_->EndFrame();

    ITitleState::EndFrame();
}

void TitleState_Out::HandOverColliders(){}

void TitleState_Out::ManageState(){
}
