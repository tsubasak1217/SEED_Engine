#include "EggState_Break.h"

//object
#include "../Egg.h"
#include "StageManager.h"

//manager
#include "ClockManager.h"
//lib
#include "JsonManager/JsonCoordinator.h"

EggState_Break::EggState_Break(BaseCharacter* character,bool breakToNextStage){
    Initialize("Break",character);
    breakToNextStage_ = breakToNextStage;

}

void EggState_Break::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    leftTime_ = breakTime_;

    JsonCoordinator::RegisterItem("Egg","BreakTime",breakTime_);
    JsonCoordinator::RegisterItem("Egg","BreakTimeFactor",timeFactor_);
}

void EggState_Break::Update(){
    if(Input::GetInstance()->IsPressPadButton(PAD_BUTTON::X)){
        leftTime_ -= ClockManager::DeltaTime() * timeFactor_;
    } else{
        leftTime_ -= ClockManager::DeltaTime();
    }

    ManageState();
}

void EggState_Break::Draw(){}

void EggState_Break::ManageState(){
    if(leftTime_ <= 0.0f){
        Egg* egg = dynamic_cast<Egg*>(pCharacter_);
        if(egg){
            egg->Break();

            if(breakToNextStage_){
                // 次のステージに進める
                StageManager::StepStage(1);
                StageManager::SetIsHandOverColliderNext(false);
                breakToNextStage_ = false;
            }
        }
    }
}