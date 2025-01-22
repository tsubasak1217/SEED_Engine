#include "PredationRange.h"

//object
#include "Player/Player.h"
#include "Enemy/Enemy.h"
//manager
#include "../Manager/EnemyManager.h"

//lib
#include "../adapter/json/JsonCoordinator.h"

//math
#include "MyMath.h"

PredationRange::PredationRange(){}

PredationRange::~PredationRange(){}

void PredationRange::Initialize(Player* player){
    player_ = player;

    // jsonから範囲を読み込む
    JsonCoordinator::RegisterItem("Player","PredationRange",range_);
    JsonCoordinator::RegisterItem("Player","PredationRangeY",rangeY_);
}

void PredationRange::Update(EnemyManager* _enemyManager){
    auto& enemies = _enemyManager->GetEnemies();
    if(enemies.empty()){
        return;
    }

    preyList_.clear();

    // 範囲内の敵を探す
    Vector3 playerPos = player_->GetWorldTranslate();
    Vector3 diffP2E;
    for(auto& enemy : enemies){
        // 捕食不可能な敵は無視
        if(!enemy->GetCanEat()){
            break;
        }
        diffP2E = enemy->GetWorldTranslate() - playerPos;
        if(MyMath::LengthSq(diffP2E) < range_ * range_){
            preyList_.push_back({enemy.get(),diffP2E});
        }
    }

    if(preyList_.empty()){
        return;
    }

    // 距離が近い順にソート
    std::sort(preyList_.begin(),preyList_.end(),[](const PreyInfomation& a,const PreyInfomation& b){
        return MyMath::LengthSq(a.diff) < MyMath::LengthSq(b.diff);
              });

    // 範囲内の敵のうち、Y軸方向の範囲内の敵を残す
    auto it = std::remove_if(preyList_.begin(),preyList_.end(),[this](const PreyInfomation& prey){
        return std::abs(prey.diff.y) > rangeY_;
                             });
    preyList_.erase(it,preyList_.end());
}
