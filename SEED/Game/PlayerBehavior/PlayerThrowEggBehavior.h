#pragma once

#include <memory>

#include "IPlayerBehavior.h"

#include "Vector3.h"

class Egg;

class PlayerThrowEggBehavior
    :public IPlayerBehavior{
public:
    PlayerThrowEggBehavior(Player* _player);
    ~PlayerThrowEggBehavior();

    void Initialize();
    void Update()override;
private:
    /// <summary>
    /// 投げた卵の速度
    /// </summary>
    // 現在値
    float currentThrowSpeed_ = 0.0f;
    // 最低値
    float minThrowSpeed_     = 2.0f;
    // 最大値
    float maxThrowSpeed_     = 8.0f;
    // 1秒で 変化する 速度値
    float updateThrowSpeedValue_ = 0.2f;

    /// <summary>
    /// 卵を投げる上方向
    /// </summary>
    // 現在値
    float currentThrowHeight_  = 3.0f;
    // 最低値
    float minThrowHeight_ = 1.0f;
    // 最大値
    float maxThrowHeight_ = 10.0f;
    // 自動で currentHeight_ が 上下が動くため,
    // 上下で 計算を 変更するために使用
    float updateSign_ = 1.0f;
    // 1秒で 変化する 高さ
    float updateThrowHeightValue_ = 0.2f;

    Egg* throwEgg_ = nullptr;

    Vector3 throwDirection_ = {0.0f,0.0f,0.0f};
};