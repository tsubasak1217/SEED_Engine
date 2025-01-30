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
   
}

void TitleState_Out::Update(){
}

void TitleState_Out::Draw(){
}

void TitleState_Out::Finalize(){
}

void TitleState_Out::BeginFrame(){
}

void TitleState_Out::EndFrame(){

    ITitleState::EndFrame();
}

void TitleState_Out::HandOverColliders(){}

void TitleState_Out::ManageState(){
}
