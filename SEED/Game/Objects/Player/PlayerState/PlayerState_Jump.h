#pragma once
#include "PlayerState_Move.h"

class PlayerState_Jump : public PlayerState_Move{
public:
    PlayerState_Jump() = default;
    PlayerState_Jump(BaseCharacter* player);
    ~PlayerState_Jump()override;
    void Update()override;
    void Draw()override;
    void Initialize(BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

private:
    void Jump();

private:
    bool isJump_ = false;
    bool isDrop_ = false;
    float jumpPower_ = 120.0f / 60.0f;
    float gravity_ = -9.8f;
    float jumpVelocity_;

};