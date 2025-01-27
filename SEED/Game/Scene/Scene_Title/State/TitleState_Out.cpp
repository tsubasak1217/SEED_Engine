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
    :ITitleState(_host){}

TitleState_Out::~TitleState_Out(){}

void TitleState_Out::Initialize(){
    //============================== whiteScreen ==============================//
    whiteScreen_ = std::make_unique<UI>("whiteScreen");
    whiteScreen_->Initialize("Assets/white1x1.png");
    whiteScreen_->SetSize({1280.f,720.f});

    // state
    {
        std::unique_ptr<UiState_LerpColor> lerpColor = std::make_unique<UiState_LerpColor>(whiteScreen_.get(),"TitleOut");
        whiteScreen_->SetState(std::move(lerpColor));
    }
}

void TitleState_Out::Update(){
    whiteScreen_->Update();
}

void TitleState_Out::Draw(){
    whiteScreen_->Draw();
}

void TitleState_Out::Finalize(){
    whiteScreen_->Finalize();
}

void TitleState_Out::BeginFrame(){
    whiteScreen_->BeginFrame();
}

void TitleState_Out::EndFrame(){
    whiteScreen_->EndFrame();
    ITitleState::EndFrame();
}

void TitleState_Out::HandOverColliders(){
}

void TitleState_Out::ManageState(){
    if(whiteScreen_->GetColor().w >= 1.f){
        host_->ChangeScene(new Scene_Game());
    }
}
