#include "PlayerState_Spawn.h"

//others state
#include "PlayerState_Idle.h"

//manager
#include "ClockManager.h"

PlayerState_Spawn::PlayerState_Spawn(){}

PlayerState_Spawn::PlayerState_Spawn(const std::string& stateName,BaseCharacter* player){
    Initialize(stateName,player);
}

PlayerState_Spawn::~PlayerState_Spawn(){}

void PlayerState_Spawn::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);
    spawnTime_ = 0.0f;
    spawnTimeLimit_ = 1.0f;
}

void PlayerState_Spawn::Update(){
    spawnTime_ += ClockManager::DeltaTime();
    ManageState();
}

void PlayerState_Spawn::Draw(){}

void PlayerState_Spawn::ManageState(){
    if(spawnTime_ >= spawnTimeLimit_){
        pCharacter_->SetTranslate(spawnPos_);
        pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
    }
}
