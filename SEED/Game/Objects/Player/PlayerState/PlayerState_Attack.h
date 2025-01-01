#pragma once
#include "Base/ICharacterState.h"

class PlayerState_Attack : public ICharacterState{
public:
    PlayerState_Attack() = default;
    PlayerState_Attack(BaseCharacter* player);
    ~PlayerState_Attack()override;
    void Update()override;
    void Draw()override;
    void Initialize(BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

    // 攻撃フェーズ管理
    void Combo();

private:

    int32_t phase_ = 1;// 攻撃フェーズ
    static const int32_t kMaxPhase_ = 3;// 最大フェーズ
    float kAttackTime_;// 固定の攻撃時間
    float attackTime_;// 残り攻撃時間
    bool moveAble_ = false;// 移動可能か

private:
    std::string animationNames_[kMaxPhase_] = {
        "punch",
        "kick",
        "spinAttack"
    };

};