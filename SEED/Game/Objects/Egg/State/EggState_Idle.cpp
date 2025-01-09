#include "EggState_Idle.h"

EggState_Idle::EggState_Idle(BaseCharacter* _egg){
    Initialize("Idle",_egg);
}

EggState_Idle::~EggState_Idle(){}

void EggState_Idle::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);
}

void EggState_Idle::Update(){}

void EggState_Idle::Draw(){}

void EggState_Idle::ManageState(){}
