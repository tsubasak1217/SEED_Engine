#pragma once
#include "Base/ICharacterState.h"

///container
#include <vector>
///math
#include <Vector3.h>

class Enemy;

class EnemyState_RoutineMove
    : public ICharacterState{
public:
    EnemyState_RoutineMove(const std::string& stateName,BaseCharacter* enemy);
    ~EnemyState_RoutineMove()override;

    void Initialize(const std::string& stateName,BaseCharacter* enemy)override;
    void Update()override;
    void Draw()override;

protected:
    // ステート管理
    void ManageState()override;

private:
    Enemy* enemy_  = nullptr;

    const  std::vector<Vector3>* movePoints_ = nullptr;
    int currentMovePointIndex_ = 0;

    float sensingDistance_ = 10.0f;
};