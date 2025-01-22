#pragma once

#include "Base/ICharacterState.h"

class Enemy;
class PlayerState_Eat
    : public ICharacterState{
public:
    PlayerState_Eat(Enemy* _enemy,BaseCharacter* player);
    ~PlayerState_Eat()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName,BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

private:
    Enemy* enemy = nullptr;
};