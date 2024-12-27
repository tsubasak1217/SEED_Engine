#pragma once
#include "Base/IState.h"

class EnemyState_Idle : public IState{
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

};