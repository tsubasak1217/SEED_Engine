#include "EggManager.h"

//Objects
#include "../Egg.h"
#include"Player/Player.h"
//lib
#include "../adapter/json/JsonCoordinator.h"

const uint32_t EggManager::maxEggsSize_ = 10;

EggManager::EggManager(){}

EggManager::~EggManager(){}

void EggManager::Initialize(){
    eggs_.reserve(maxEggsSize_);

    JsonCoordinator::LoadGroup("Egg");
}

void EggManager::Update(){

    std::erase_if(eggs_,[](std::unique_ptr<Egg>& egg){
        return egg->GetIsBreak() || !egg->GetIsAlive();
                  });

    for(auto& egg : eggs_){
        egg->Update();
        egg->EditCollider();
    }
}

void EggManager::Draw(){
    for(auto& egg : eggs_){
        egg->Draw();
    }
}

void EggManager::HandOverColliders(){
    for(auto& egg : eggs_){
        egg->HandOverColliders();
    }
}

void EggManager::BeginFrame(){
#ifdef _DEBUG
    ImGui::Begin("Egg");
    JsonCoordinator::RenderGroupUI("Egg");
    if(ImGui::Button("Save")){
        JsonCoordinator::SaveGroup("Egg");
    }
    ImGui::End();
#endif // _DEBUG

    for(auto& egg : eggs_){
        egg->BeginFrame();
    }
}

void EggManager::EndFrame(){
    for(auto& egg : eggs_){
        egg->EndFrame();
    }
}

void EggManager::InitializeEggCount(){
    if(!eggs_.empty()){
        eggs_.clear();
    }

    auto newEgg = std::make_unique<Egg>(player_);
    newEgg->SetEggManager(this);
    newEgg->Initialize();
    eggs_.push_back(std::move(newEgg));
}

void EggManager::AddEgg(std::unique_ptr<Egg>& _egg){
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

std::unique_ptr<Egg>& EggManager::GetBackEgg(){
    return eggs_.back();
}

bool EggManager::GetIsEmpty() const{
    return eggs_.empty();
}
