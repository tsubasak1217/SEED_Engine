#include "PlayerCorpseManager.h"

#include "PlayerCorpse/PlayerCorpse.h"

PlayerCorpseManager::PlayerCorpseManager(){}

PlayerCorpseManager::~PlayerCorpseManager(){}

void PlayerCorpseManager::Initialize(){
    playerCorpses_.reserve(100);
}

void PlayerCorpseManager::Update(){
    for(auto& corpse : playerCorpses_){
        corpse->Update();
    }
}

void PlayerCorpseManager::Draw(){
    for(auto& corpse : playerCorpses_){
        corpse->Draw();
    }
}

void PlayerCorpseManager::AddPlayerCorpse(std::unique_ptr<PlayerCorpse>& addCorpse){
    playerCorpses_.push_back(std::move(addCorpse));
}