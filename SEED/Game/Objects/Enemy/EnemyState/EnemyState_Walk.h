#pragma once
#include "Base/ICharacterState.h"

class EnemyState_Walk : public ICharacterState{
public:
    EnemyState_Walk() = default;
    EnemyState_Walk(const std::string& stateName, BaseCharacter* enemy);
    ~EnemyState_Walk()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName, BaseCharacter* enemy)override;

protected:
    void Move();
    void Rotate();

protected:
    // ステート管理
    void ManageState()override;

private:
    // 回転の補間
    bool isLerpRotate_ = false;
    float lerpRate_ = 0.25f;
    // 攻撃範囲
    float attackRange_ = 15.0f;
    // 移動情報
    float moveSpeed_ = 7.0f;
};