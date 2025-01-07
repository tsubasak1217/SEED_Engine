#pragma once
#include <memory>

#include "IPlayerBehavior.h"

#include "Vector3.h"

class Egg;

class PlayerSpawnFromEgg
    :public IPlayerBehavior{
public:
    PlayerSpawnFromEgg(Player* _player);
    ~PlayerSpawnFromEgg();

    void Initialize();
    void Update()override;
private:
    Egg* egg_ = nullptr;
};
