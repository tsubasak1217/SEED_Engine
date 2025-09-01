#include "GameState_Play.h"
#include <SEED/Source/SEED.h>

GameState_Play::GameState_Play(Scene_Base* pScene){
    // シーンの設定
    pScene_ = pScene;
}


GameState_Play::~GameState_Play(){
    SEED::SetMainCamera("default");
    AudioManager::EndAllAudio();
}

void GameState_Play::Initialize(){
}


void GameState_Play::Finalize(){
}

void GameState_Play::Update(){
}

void GameState_Play::Draw(){
}

void GameState_Play::BeginFrame(){
}

void GameState_Play::EndFrame(){
}

void GameState_Play::HandOverColliders(){
}

void GameState_Play::ManageState(){
}
