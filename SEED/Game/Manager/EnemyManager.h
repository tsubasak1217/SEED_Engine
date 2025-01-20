#pragma once

// local
#include "../Enemy/Enemy.h"

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
    void DeleteEnemy(uint32_t index);

    //--- getter / setter ---//
    std::vector<std::unique_ptr<Enemy>>& GetEnemies(){ return enemies_; }
    void ClearAllEnemies(){ enemies_.clear(); }
    Player* GetPlayer()const{ return pPlayer_; }

    const  std::vector<Vector3>* GetRoutinePoints(const std::string& routineName)const{
        auto itr = routinePointsLibrary_.find(routineName);
        if(itr == routinePointsLibrary_.end()){ assert(false); }
        return &(itr->second);
    }


    std::unordered_map<std::string, std::vector<Vector3>>& GetRoutineLibrary(){ return routinePointsLibrary_; }
private:
    std::vector<std::unique_ptr<Enemy>> enemies_;

    // 固定移動 ポイント 
    std::unordered_map<std::string,std::vector<Vector3>> routinePointsLibrary_;

    Player* pPlayer_ = nullptr;
};