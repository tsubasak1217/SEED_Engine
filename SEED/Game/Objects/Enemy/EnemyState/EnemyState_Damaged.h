#pragma once
#include "Base/ICharacterState.h"

class EnemyState_Damaged : public ICharacterState{
public:
    EnemyState_Damaged() = default;
    EnemyState_Damaged(const std::string& stateName, BaseCharacter* enemy);
    ~EnemyState_Damaged()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName, BaseCharacter* enemy)override;

protected:
    // ステート管理
    void ManageState()override;

private:

};