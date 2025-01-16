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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//      enemyの追加
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::AddEnemy(){
    if (!pPlayer_){ assert(false); }
    //enemyの生成
    auto newEnemy = std::make_unique<Enemy>(this,pPlayer_,"Enemy");
    enemies_.emplace_back(std::move(newEnemy));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//      enemyの削除
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::DeleteEnemy(uint32_t index){
    if (index < enemies_.size()){
        enemies_.erase(enemies_.begin() + index);
    }
}