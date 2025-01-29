#pragma once
#include "PlayerState_Move.h"

enum class HoveringState{
    MoveUp,
    Hovering,
    MoveDown,
};

class PlayerState_Jump : public PlayerState_Move{
public:
    PlayerState_Jump() = default;
    PlayerState_Jump(const std::string& stateName,BaseCharacter* player);
    ~PlayerState_Jump()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName,BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

private:
    float jumpPower_ = 0.f;
    float jumpHoveringAccel_ = 0.f;

    HoveringState hoveringState_ = HoveringState::MoveUp;
    
    float hoveringTime_ = 0.f;
    float leftTime_ = 0.f;

};