#pragma once

// local
#include "../Enemy/Enemy.h"

//lib
#include <vector>
#include <memory>
#include <cstdint>

class Player;

class EnemyManager{
public:
    EnemyManager();
    EnemyManager(Player* player);
    ~EnemyManager() = default;

    void Initialize();
    void Update();
    void Draw();

    void AddEnemy();
    void DeleteEnemy(uint32_t index);

    //--- getter / setter ---//
    std::vector<std::unique_ptr<Enemy>>& GetEnemies(){ return enemies_; }
    void ClearAllEnemies(){ enemies_.clear(); }
    Player* GetPlayer()const{ return pPlayer_; }

private:
    std::vector<std::unique_ptr<Enemy>> enemies_;
    Player* pPlayer_ = nullptr;
};