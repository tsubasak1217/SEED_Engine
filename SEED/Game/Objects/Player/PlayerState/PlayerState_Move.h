#pragma once
#include "../Game/Objects/Player/PlayerState/IPlayerState.h"

class PlayerState_Move : public IPlayerState{
public:
    PlayerState_Move() = default;
    PlayerState_Move(Player* player);
    ~PlayerState_Move()override;
    void Update()override;
    void Draw()override;
    void Initialize(Player* player)override;

protected:
    // ステート管理
    void ManageState()override;

protected:
    // 移動処理
    void Move();

private:
    void DecideStickVelocity();
    void Rotate();

private:
    float moveSpeed_ = 30.0f;
    Vector2 stickDirection_;
    Vector3 acceleration_;
    Vector3 moveVec_;
};