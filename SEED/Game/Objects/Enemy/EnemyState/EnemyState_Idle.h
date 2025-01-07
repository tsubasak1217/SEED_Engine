#pragma once
#include "Base/ICharacterState.h"

class EnemyState_Idle : public ICharacterState{
public:
    EnemyState_Idle() = default;
    EnemyState_Idle(BaseCharacter* enemy);
    ~EnemyState_Idle()override;
    void Update()override;
    void Draw()override;
    void Initialize(BaseCharacter* enemy)override;

protected:
    // ステート管理
    void ManageState()override;

private:
    float sensingDistance_ = 60.0f;// 感知距離
};