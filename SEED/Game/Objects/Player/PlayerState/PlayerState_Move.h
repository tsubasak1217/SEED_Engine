#pragma once
#include "Base/ICharacterState.h"

class PlayerState_Move : public ICharacterState{
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
    // 回転の補間
    bool isLerpRotate_ = true;
    float lerpRate_ = 0.25f;
    // 移動情報
    float moveSpeed_ = 30.0f;
    Vector2 stickDirection_;
    Vector3 acceleration_;
    Vector3 moveVec_;
};