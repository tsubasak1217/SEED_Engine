#pragma once
#include "../Game/Objects/Player/Player.h"
#include "InputManager.h"

class IPlayerState{
public:
    IPlayerState() = default;
    IPlayerState(Player* player){ player; }
    virtual ~IPlayerState() = default;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Initialize(Player* player){ pPlayer_ = player; }

protected:
    virtual void ManageState() = 0;

protected:
    Player* pPlayer_;
};