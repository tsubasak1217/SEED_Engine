#pragma once

#include "IPlayerBehavior.h"

////////////////////////////////////////////////////
//  Player の 移動 
////////////////////////////////////////////////////
class PlayerRootBehavior
    :public IPlayerBehavior{
public:
    PlayerRootBehavior(Player* _player);
    ~PlayerRootBehavior();

    void Update()override;
private:
    float moveSpeed_ = 3.0f;
};