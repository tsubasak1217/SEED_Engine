#include "EggState_Break.h"

//object
#include "../Egg.h"

//manager
#include "ClockManager.h"

EggState_Break::EggState_Break(BaseCharacter* character){
    pCharacter_ = character;
}

void EggState_Break::Initialize(BaseCharacter* character){
    ICharacterState::Initialize(character);

    leftTime_ = breakTime_;
}

void EggState_Break::Update(){
    leftTime_ -= ClockManager::DeltaTime();

    ManageState();
}

void EggState_Break::Draw(){}

void EggState_Break::ManageState(){
    if(leftTime_ <= 0.0f){
        Egg* egg = dynamic_cast<Egg*>(pCharacter_);
        if(egg){
            egg->Break();
        }
    }
}
