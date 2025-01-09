#include "EnemyManager.h"

EnemyManager::EnemyManager(){
    enemies_.clear();
}

EnemyManager::EnemyManager(Player* player)
:pPlayer_(player){}



void EnemyManager::Initialize(){}

void EnemyManager::Update(){
    for (auto& enemy : enemies_){
        enemy->Update();
    }
}

void EnemyManager::Draw(){
    for (auto& enemy : enemies_){
        enemy->Draw();
    }
}

void EnemyManager::AddEnemy(){
    if (pPlayer_){ assert(false); }
    //enemyの生成
    auto newEnemy = std::make_unique<Enemy>(pPlayer_);
    enemies_.emplace_back(std::move(newEnemy));
}
