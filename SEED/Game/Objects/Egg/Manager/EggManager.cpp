#include "EggManager.h"

//Objects
#include "../Egg.h"
#include"Player/Player.h"
//lib
#include "JsonManager/JsonCoordinator.h"

const uint32_t EggManager::maxEggsSize_ = 10;

EggManager::EggManager(){}

EggManager::~EggManager(){}

void EggManager::Initialize(){
    eggs_.reserve(maxEggsSize_);

    eggs_.push_back(std::make_unique<Egg>(player_));
    auto& spawnedEgg = eggs_.back();
    spawnedEgg->SetEggManager(this);
    spawnedEgg->Initialize();
}

void EggManager::Update(){
    std::erase_if(eggs_,[](std::unique_ptr<Egg>& egg){
        return egg->GetIsBreak();
                  });

    JsonCoordinator::RenderGroupUI("Egg");

    if(eggs_.empty()){
        eggs_.push_back(std::make_unique<Egg>(player_));
        auto& spawnedEgg = eggs_.back();
        spawnedEgg->SetEggManager(this);
        spawnedEgg->Initialize();
    }

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
    _egg->SetEggManager(this);
    eggs_.push_back(std::move(_egg));
}

void EggManager::RemoveEgg(Egg* _egg){
    auto itr = std::find_if(eggs_.begin(),eggs_.end(),[_egg](const std::unique_ptr<Egg>& egg){
        return egg.get() == _egg;
                            });
    if(itr != eggs_.end()){
        eggs_.erase(itr);
    }
}

std::unique_ptr<Egg>& EggManager::GetFrontEgg(){
    return eggs_.front();
}