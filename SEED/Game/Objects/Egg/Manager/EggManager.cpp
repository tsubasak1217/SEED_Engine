#include "EggManager.h"

//Objects
#include "../Egg.h"
#include"Player/Player.h"

const uint32_t EggManager::maxEggsSize_ = 10;

EggManager::EggManager(){}

EggManager::~EggManager(){}

void EggManager::Initialize(){
    eggs_.reserve(maxEggsSize_);

    eggs_.push_back(std::make_unique<Egg>(player_));
    eggs_.back()->Initialize();
}

void EggManager::Update(){
    for(auto& egg : eggs_){
        egg->Update();
    }
}

void EggManager::Draw(){
    for(auto& egg : eggs_){
        egg->Draw();
    }
}

void EggManager::AddEgg(std::unique_ptr<Egg> _egg){
    /// 最大サイズ なら Skip
    if(eggs_.size() >= maxEggsSize_){
        return;
    }
    eggs_.push_back(std::move(_egg));
}

std::unique_ptr<Egg>& EggManager::GetFrontEgg(){
    return eggs_.front();
}