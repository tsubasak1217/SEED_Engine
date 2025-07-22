#include "GameState_Strolling.h"
#include <SEED/Source/Basic/Scene/Scene_Base.h>

GameState_Strolling::GameState_Strolling(){

}

GameState_Strolling::GameState_Strolling(Scene_Base* pScene){
    pScene_ = pScene;
    pScene_->GetHierarchy()->LoadFromJson("Resources/Jsons/Scenes/scene.json");
}

void GameState_Strolling::Initialize(){

}

void GameState_Strolling::Finalize(){
}

void GameState_Strolling::Update(){
}

void GameState_Strolling::Draw(){
}

void GameState_Strolling::BeginFrame(){
}

void GameState_Strolling::EndFrame(){
}

void GameState_Strolling::HandOverColliders(){
}

void GameState_Strolling::ManageState(){
}
