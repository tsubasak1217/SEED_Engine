#include "PlayerState_Eat.h"

#include "Enemy/Enemy.h"

PlayerState_Eat::PlayerState_Eat(Enemy* _enemy,BaseCharacter* player){
    enemy = _enemy;
    Initialize("PlayerState_Eat",player);
}

PlayerState_Eat::~PlayerState_Eat(){}

void PlayerState_Eat::Update(){}

void PlayerState_Eat::Draw(){}

void PlayerState_Eat::Initialize(const std::string& stateName,BaseCharacter* player){
    ICharacterState::Initialize(stateName,player);


}
