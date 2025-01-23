#include "EnemyManager.h"
#include "Sprite.h"
#include "SEED.h"

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
    Sprite sprite("Assets/white1x1.png");
    sprite.size = { 30,30 };
    sprite.anchorPoint = { 0.5f,0.5f };
    for (auto& enemy : enemies_){
        enemy->Draw();
        sprite.translate = SEED::GetCamera()->ToScreenPosition(enemy->GetWorldTranslate());
        SEED::DrawSprite(sprite);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//      enemyの追加
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::AddEnemy(){
    if (!pPlayer_){ assert(false); }
    //enemyの生成
    const std::string enemyName = "Enemy" + std::to_string(( int ) enemies_.size());
    auto newEnemy = std::make_unique<Enemy>(this,pPlayer_, enemyName);
    enemies_.emplace_back(std::move(newEnemy));
}

void EnemyManager::AddEnemy(std::unique_ptr<Enemy>enemy){
    if (!pPlayer_){ assert(false); }
    enemies_.emplace_back(std::move(enemy));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//      enemyの削除
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::DeleteEnemy(uint32_t index){
    if (index < enemies_.size()){
        enemies_.erase(enemies_.begin() + index);
    }
}

void EnemyManager::HandOverColliders(){
    for (auto& enemy:enemies_){
        enemy->HandOverColliders();
    }
}
