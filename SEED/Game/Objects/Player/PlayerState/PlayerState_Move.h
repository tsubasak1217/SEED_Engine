#pragma once
#include "Base/IState.h"

class PlayerState_Move : public IState{
public:
    PlayerState_Move() = default;
    PlayerState_Move(BaseCharacter* player);
    ~PlayerState_Move()override;
    void Update()override;
    void Draw()override;
    void Initialize(BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

protected:
    // 移動処理
    void Move();
    void Rotate();

private:
    void DecideStickVelocity();

private:
    float moveSpeed_ = 30.0f;
    Vector2 stickDirection_;
    Vector3 acceleration_;
    Vector3 moveVec_;
};