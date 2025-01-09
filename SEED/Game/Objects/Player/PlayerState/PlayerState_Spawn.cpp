#include "PlayerState_Spawn.h"

//others state
#include "PlayerState_Idle.h"

//manager
#include "ClockManager.h"

PlayerState_Spawn::PlayerState_Spawn(){}

PlayerState_Spawn::PlayerState_Spawn(BaseCharacter* character,const Vector3& _spawnPos){
    pCharacter_ = character;
    spawnPos_ = _spawnPos;
}

PlayerState_Spawn::~PlayerState_Spawn(){}

void PlayerState_Spawn::Initialize(BaseCharacter* character){
    ICharacterState::Initialize(character);
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
        pCharacter_->ChangeState(new PlayerState_Idle(pCharacter_));
    }
}
