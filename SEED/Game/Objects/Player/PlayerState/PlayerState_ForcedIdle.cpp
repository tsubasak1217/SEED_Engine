#include "PlayerState_ForcedIdle.h"

PlayerState_ForcedIdle::PlayerState_ForcedIdle(BaseCharacter *_pCharacter)
{
    Initialize("PlayerState_ForcedIdle", _pCharacter);
}

PlayerState_ForcedIdle::~PlayerState_ForcedIdle()
{
}

void PlayerState_ForcedIdle::Initialize(const std::string &stateName, BaseCharacter *player)
{
    PlayerState_Idle::Initialize(stateName, player);
}

void PlayerState_ForcedIdle::Update()
{
    PlayerState_Idle::Update();
}

void PlayerState_ForcedIdle::Draw()
{
}

void PlayerState_ForcedIdle::ManageState()
{
}
