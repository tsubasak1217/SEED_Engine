#pragma once

#include <memory>

#include "PlayerState_Move.h"

#include "Vector3.h"

#include "../Objects/Egg/Egg.h"
#include "../Objects/Egg/Manager/EggManager.h"

class PlayerState_ThrowEgg
    :public PlayerState_Move{
public:
    PlayerState_ThrowEgg(
        BaseCharacter* _player,
        EggManager* _eggManager);
    ~PlayerState_ThrowEgg();

    void Initialize(BaseCharacter* character)override;
    void Update()override;
    void Draw()override;
private:
    void ManageState()override;
private:
    EggManager* eggManager_ = nullptr;
    Egg* throwEgg_ = nullptr;

    // 投げる卵のオフセット
    Vector3 eggOffset_ = {0.0f,0.0f,0.0f};
    // 投げる方向
    Vector3 throwDirection_ = {0.0f,0.0f,0.0f};
    // 投げるパワー
    const float throwPower_ = 0.0f;
    // 飛んでいく上方向 は 固定
    const float throwRotateX_ = 4.0f;
};