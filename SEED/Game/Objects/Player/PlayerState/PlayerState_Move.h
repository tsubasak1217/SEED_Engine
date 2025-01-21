#pragma once
#include "Base/ICharacterState.h"

class PlayerState_Move : public ICharacterState{
public:
    PlayerState_Move() = default;
    PlayerState_Move(const std::string& stateName,BaseCharacter* player);
    ~PlayerState_Move()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName,BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

protected:
    // 移動処理
    void Move();
    void Rotate();

private:
    void DecideStickVelocity();
protected:
    // 移動情報
    float moveSpeed_ = 30.0f;
private:
    // 回転の補間
    float lerpRate_ = 0.25f;
    bool isLerpRotate_ = true;
    Vector2 stickDirection_;
    Vector3 acceleration_;
    Vector3 moveVec_;
};