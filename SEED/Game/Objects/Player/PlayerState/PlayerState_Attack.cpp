#include "PlayerState_Attack.h"

PlayerState_Attack::PlayerState_Attack(Player* player){
    Initialize(player);
}

PlayerState_Attack::~PlayerState_Attack(){}

void PlayerState_Attack::Update(){}

void PlayerState_Attack::Draw(){}

void PlayerState_Attack::Initialize(Player* player){
    IPlayerState::Initialize(player);
}

void PlayerState_Attack::ManageState(){}
