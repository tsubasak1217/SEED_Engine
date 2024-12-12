#pragma once

#include "IPlayerBehavior.h"

////////////////////////////////////////////////////
//  Player の 移動 ()
////////////////////////////////////////////////////
class PlayerRootBehavior
    :public IPlayerBehavior{
public:
    PlayerRootBehavior(Player* _player);
    ~PlayerRootBehavior();

    void Initialize()override;
    void Update()override;
private:
    /// <summary>
    /// 移動処理
    /// </summary>
    void MoveUpdate();
    /// <summary>
    /// Input に よる Behavior の 変更 
    /// </summary>
    void UpdateBehaviorByInput();

private:
    float moveSpeed_ = 3.0f;
};