#include "GameState_Pause.h"
#include <SEED/Source/Basic/Scene/Scene_Base.h>

GameState_Pause::GameState_Pause(){

}

GameState_Pause::GameState_Pause(Scene_Base* pScene){
    pScene_ = pScene;
    SEED::SetMainCamera("debug");
}

void GameState_Pause::Initialize(){

}

void GameState_Pause::Finalize(){
}

void GameState_Pause::Update(){
}

void GameState_Pause::Draw(){
}

void GameState_Pause::BeginFrame(){
}

void GameState_Pause::EndFrame(){
}

void GameState_Pause::HandOverColliders(){
}

void GameState_Pause::ManageState(){
}
