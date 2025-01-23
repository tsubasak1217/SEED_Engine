#pragma once

// parent
#include "Base/ICharacterState.h"

//stl
//pointer
#include <functional>

class Enemy;
class PlayerState_Eat
    : public ICharacterState{
public:
    PlayerState_Eat(BaseCharacter* player);
    ~PlayerState_Eat()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName,BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

    void RotateForEnemy();
    void EatEnemy();
    void SpawnEgg();
private:
    // アニメーションごとにステートを分ける
    std::function<void()> currentUpdate_ = nullptr;

    Enemy* enemy_ = nullptr;

    float rotateTime_ = 0.06f;
    float interpolationRotateY_;

    float eatTime_ = 0.1f;

    float spawnEggTime_ = 0.01f;

    float currentTime_;
};