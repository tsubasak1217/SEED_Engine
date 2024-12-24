#pragma once
#include "../Game/Objects/Player/PlayerState/IPlayerState.h"

class PlayerState_Attack : public IPlayerState{
public:
    PlayerState_Attack() = default;
    PlayerState_Attack(Player* player);
    ~PlayerState_Attack()override;
    void Update()override;
    void Draw()override;
    void Initialize(Player* player)override;

protected:
    // ステート管理
    void ManageState()override;
};