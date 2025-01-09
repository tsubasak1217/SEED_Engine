#pragma once
#include "Base/ICharacterState.h"

class EnemyState_Attack : public ICharacterState{
public:
    EnemyState_Attack() = default;
    EnemyState_Attack(const std::string& stateName, BaseCharacter* enemy);
    ~EnemyState_Attack()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName, BaseCharacter* enemy)override;

protected:
    // ステート管理
    void ManageState()override;

private:

};