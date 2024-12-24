#pragma once
#include "../Game/Objects/Player/PlayerState/IPlayerState.h"

class PlayerState_Idle : public IPlayerState{
public:
    PlayerState_Idle() = default;
    PlayerState_Idle(Player* player);
    ~PlayerState_Idle()override;
    void Update()override;
    void Draw()override;
    void Initialize(Player* player)override;

protected:
    // ステート管理
    void ManageState()override;

private:

};