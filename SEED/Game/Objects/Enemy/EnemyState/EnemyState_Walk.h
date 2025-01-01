#pragma once
#include "Base/ICharacterState.h"

class EnemyState_Walk : public ICharacterState{
public:
    EnemyState_Walk() = default;
    EnemyState_Walk(BaseCharacter* enemy);
    ~EnemyState_Walk()override;
    void Update()override;
    void Draw()override;
    void Initialize(BaseCharacter* enemy)override;

protected:
    // ステート管理
    void ManageState()override;

private:
    float attackRange_ = 100.0f;
};