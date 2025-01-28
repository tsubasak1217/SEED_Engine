#pragma once

// local
#include "../Enemy/Enemy.h"
#include "../Routine/RoutineManager.h"
//lib
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Player;

class EnemyManager{
public:
    EnemyManager() = default;
    EnemyManager(Player* player,uint32_t stageNo,RoutineManager& routineManager);
    ~EnemyManager() = default;

    void Initialize();
    void Update();
    void Draw();

    void AddEnemy();
    void AddEnemy(std::unique_ptr<Enemy>);
    void DeleteEnemy(uint32_t index);
    void HandOverColliders();

    //--- 保存 読み込み ---//
    void SaveEnemies();
    void LoadEnemies();

    //--- getter / setter ---//
    std::vector<std::unique_ptr<Enemy>>& GetEnemies(){ return enemies_; }
    void ClearAllEnemies(){ enemies_.clear(); }
    Player* GetPlayer()const{ return pPlayer_; }
    Enemy* GetEnemy(uint32_t index)const{ return enemies_[index].get(); }

    // ルーチンの取得
    const  std::vector<Vector3>* GetRoutinePoints(const std::string& routineName)const{
        return routineManager_.GetRoutinePoints(routineName);
    }

    // ルーチン名の取得
    std::vector<std::string> GetRoutineNames()const{
        return routineManager_.GetRoutineNames();
    }

    // ルーチン管理
    RoutineManager* GetRoutineManager(){ return &routineManager_; }

    void SetPlayer(Player* player){ pPlayer_ = player; }

private:
    std::vector<std::unique_ptr<Enemy>> enemies_;

    // 固定移動 ポイント 
    RoutineManager& routineManager_;

    Player* pPlayer_ = nullptr;

    int32_t enemyCount_ = 0;

    int32_t stageNo_ = 0;
};