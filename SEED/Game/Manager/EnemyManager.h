#pragma once

// local
#include "../Enemy/Enemy.h"
#include "../Enemy/Routine/EnemyRoutineManager.h"
//lib
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
    void AddEnemy(std::unique_ptr<Enemy>);
    void DeleteEnemy(uint32_t index);
    void HandOverColliders();

    //--- getter / setter ---//
    std::vector<std::unique_ptr<Enemy>>& GetEnemies(){ return enemies_; }
    void ClearAllEnemies(){ enemies_.clear(); }
    Player* GetPlayer()const{ return pPlayer_; }

    // ルーチンの取得
    const  std::vector<Vector3>* GetRoutinePoints(const std::string& routineName)const{
        return routineManager_.GetRoutinePoints(routineName);
    }

    // ルーチン名の取得
    std::vector<std::string> GetRoutineNames()const{
        return routineManager_.GetRoutineNames();
    }

    // ルーチン管理
    EnemyRoutineManager* GetRoutineManager(){ return &routineManager_; }

    void SetPlayer(Player* player){ pPlayer_ = player; }

private:
    std::vector<std::unique_ptr<Enemy>> enemies_;

    // 固定移動 ポイント 
    EnemyRoutineManager routineManager_;

    Player* pPlayer_ = nullptr;
};