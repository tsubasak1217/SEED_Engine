#include "TitleState_Main.h"

//parent
#include "Scene_Title.h"

//othersState
#include "TitleState_Out.h"

TitleState_Main::TitleState_Main(Scene_Title* _host)
    :ITitleState(_host){}

TitleState_Main::~TitleState_Main(){}

void TitleState_Main::Initialize(){
    input_ = Input::GetInstance();
}

void TitleState_Main::Update(){
}

void TitleState_Main::Draw(){}

void TitleState_Main::Finalize(){}

void TitleState_Main::BeginFrame(){}

void TitleState_Main::EndFrame(){
    ITitleState::EndFrame();
}

void TitleState_Main::HandOverColliders(){}

void TitleState_Main::ManageState(){
    if(input_->IsPressAnyKey()){
        host_->ChangeState(new TitleState_Out(host_));
        return;
    }
}
