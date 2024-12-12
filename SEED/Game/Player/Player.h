#pragma once

#include <memory>

#include "../GameObject/GameObject.h"

class IPlayerBehavior;
class Egg;
class EggManager;

class Player
    : public GameObject{
public:
    Player();
    ~Player();

    void Initialize()override;
    void Update()override;

    //複数の ビヘイビア で使用するため Playerに 持たせる
     /// <summary>
     /// 入力された値を に Player の 方向 を 更新
     /// </summary>
     /// <returns></returns>
    void RotateUpdate();
private:
    std::unique_ptr<IPlayerBehavior> currentBehavior_;

    EggManager* eggManager_ = nullptr;
public:
    EggManager* GetEggManager()const;
    void SetEggManager(EggManager* _eggManager);

    /// <summary>
    /// Behavior を 変更，初期化
    /// </summary>
    /// <param name="_nextBehavior"></param>
    void ChangeBehavior(std::unique_ptr<IPlayerBehavior> _nextBehavior);
};