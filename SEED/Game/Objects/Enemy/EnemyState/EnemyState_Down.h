#pragma once
#include "Base/ICharacterState.h"

class EnemyState_Down : public ICharacterState{
public:
    EnemyState_Down() = default;
    EnemyState_Down(BaseCharacter* enemy);
    ~EnemyState_Down()override;
    void Update()override;
    void Draw()override;
    void Initialize(BaseCharacter* enemy)override;

protected:
    // ステート管理
    void ManageState()override;

private:

};