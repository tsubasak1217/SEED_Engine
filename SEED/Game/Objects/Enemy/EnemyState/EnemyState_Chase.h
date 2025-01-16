#pragma once

#include "Base/ICharacterState.h"

class Enemy;

class EnemyState_Chase
    : public ICharacterState{
public:
    EnemyState_Chase(const std::string& stateName,BaseCharacter* enemy);
    ~EnemyState_Chase()override;

    void Initialize(const std::string& stateName,BaseCharacter* enemy)override;
    void Update()override;
    void Draw()override;

protected:
    // ステート管理
    void ManageState()override;

private:
    Enemy* enemy_  = nullptr;

    float sensingDistance_ = 0.0f;
    float speed_ = 0.0f;
};