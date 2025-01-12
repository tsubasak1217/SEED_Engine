#pragma once
#include "PlayerState_Move.h"

class PlayerState_Jump : public PlayerState_Move{
public:
    PlayerState_Jump() = default;
    PlayerState_Jump(const std::string& stateName, BaseCharacter* player);
    ~PlayerState_Jump()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName, BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

private:
    void Jump();

private:
    float jumpPower_ = 120.0f / 60.0f;

};