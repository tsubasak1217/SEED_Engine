#pragma once
#include "Base/ICharacterState.h"

class EnemyState_Damaged : public ICharacterState{
public:
    EnemyState_Damaged() = default;
    EnemyState_Damaged(BaseCharacter* enemy);
    ~EnemyState_Damaged()override;
    void Update()override;
    void Draw()override;
    void Initialize(BaseCharacter* enemy)override;

protected:
    // ステート管理
    void ManageState()override;

private:

};