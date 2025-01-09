#pragma once

#include "../Enemy/Enemy.h"

#include <list>
#include <memory>

class Player;

class EnemyManager{
public:
    EnemyManager();
    EnemyManager(Player* player);
    ~EnemyManager() = default;

    void Initialize();
    void Update();
    void Draw();

private:
    void AddEnemy();

private:
    std::list<std::unique_ptr<Enemy>> enemies_;
    Player* pPlayer_ = nullptr;
};