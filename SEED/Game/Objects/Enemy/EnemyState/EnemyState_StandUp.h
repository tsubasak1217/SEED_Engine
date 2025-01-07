#pragma once
#include "Base/ICharacterState.h"

class EnemyState_StandUp : public ICharacterState{
public:
    EnemyState_StandUp() = default;
    EnemyState_StandUp(BaseCharacter* enemy);
    ~EnemyState_StandUp()override;
    void Update()override;
    void Draw()override;
    void Initialize(BaseCharacter* enemy)override;

protected:
    // ステート管理
    void ManageState()override;

private:

};