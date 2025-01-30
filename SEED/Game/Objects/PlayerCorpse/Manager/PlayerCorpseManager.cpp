#include "PlayerCorpseManager.h"

#include "PlayerCorpse/PlayerCorpse.h"

PlayerCorpseManager::PlayerCorpseManager(){}

PlayerCorpseManager::~PlayerCorpseManager(){}

void PlayerCorpseManager::Initialize(){
    playerCorpses_.reserve(100);
}

void PlayerCorpseManager::Update(){
    std::erase_if(playerCorpses_,[](std::unique_ptr<PlayerCorpse>& corpse){
        return !corpse->GetIsAlive();
                  });

    for(auto& corpse : playerCorpses_){
        corpse->Update();
        corpse->EditCollider();
    }
}

void PlayerCorpseManager::Draw(){
    for(auto& corpse : playerCorpses_){
        corpse->Draw();
    }
}

void PlayerCorpseManager::HandOverColliders(){
    for (auto& corpse : playerCorpses_){
        corpse->HandOverColliders();
    }
}

void PlayerCorpseManager::BeginFrame(){
    for(auto& corpse : playerCorpses_){
        corpse->BeginFrame();
    }
}

void PlayerCorpseManager::EndFrame(){
    for(auto& corpse : playerCorpses_){
        corpse->EndFrame();
    }
}

void PlayerCorpseManager::AddPlayerCorpse(std::unique_ptr<PlayerCorpse>& addCorpse){
    playerCorpses_.push_back(std::move(addCorpse));
}

void PlayerCorpseManager::RemoveAll(){
    playerCorpses_.clear();
}
