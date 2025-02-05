#include "PredationRange.h"

// object
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "Sprite.h"
// manager
#include "../Manager/EnemyManager.h"

// lib
#include "../adapter/json/JsonCoordinator.h"

// math
#include "MyMath.h"

PredationRange::PredationRange(){}

PredationRange::~PredationRange(){}

void PredationRange::Initialize(Player* player){
    player_ = player;

    buttonUI_ = std::make_unique<Sprite>("GameUI/B.png");
    buttonUI_->anchorPoint = Vector2(0.5f,0.5f);

    // jsonから範囲を読み込む
    JsonCoordinator::RegisterItem("Player","PredationRangeXZ",rangeXZ_);
    JsonCoordinator::RegisterItem("Player","PredationRangeY",rangeY_);
    JsonCoordinator::RegisterItem("Player","PredationCatchAngle",catchAngle_);
}

void PredationRange::Update(EnemyManager* _enemyManager){
    auto& enemies = _enemyManager->GetEnemies();

    if(!preyList_.empty()){
        preyList_.clear();
    }

    if(enemies.empty()){
        return;
    }

    // 範囲内の敵を探す
    Vector3 playerPos = player_->GetWorldTranslate();
    Vector3 playerDirection = Vector3(0.0f,0.0f,1.0f) * RotateYMatrix(player_->GetWorldRotate().y);
    playerDirection = MyMath::Normalize(playerDirection);

    float localRangeY = rangeY_ * player_->GetWorldScale().y;
    float localRangeXZ = rangeXZ_ * player_->GetWorldScale().z;

    Vector3 diffP2E;
    Vector3 directionP2E;
    for(auto& enemy : enemies){
        // 捕食不可能な敵は無視
        if(!enemy->GetCanEat()){
            continue;
        }
        diffP2E = enemy->GetWorldTranslate() - playerPos;
        if(MyMath::LengthSq({diffP2E.x,diffP2E.z}) < localRangeXZ * localRangeXZ){
            directionP2E = MyMath::Normalize(diffP2E);

            // 許容範囲外の敵は無視
            if(MyMath::Cross(Vector2(playerDirection.x,playerDirection.z),Vector2(directionP2E.x,directionP2E.z)) > static_cast<float>(catchAngle_)){
                continue;
            }
            preyList_.push_back({enemy.get(),diffP2E});
        }
    }

    if(preyList_.empty()){
        return;
    }

    // 距離が近い順にソート
    preyList_.sort([](const PreyInfomation& a,const PreyInfomation& b){ return MyMath::LengthSq(a.diff) < MyMath::LengthSq(b.diff); });

    // 範囲内の敵のうち、Y軸方向の範囲内の敵を残す
    std::erase_if(preyList_,[this, localRangeY](const PreyInfomation& prey){ return std::abs(prey.diff.y) > localRangeY; });
}

void PredationRange::Draw(){
    if(preyList_.empty()){
        return;
    }
    auto& targetEnemy_ = preyList_.front().enemy;

    if(!targetEnemy_){
        return;
    }

    buttonUI_->translate = player_->GetFollowCamera()->ToScreenPosition(targetEnemy_->GetWorldTranslate());
    buttonUI_->translate.y += 10.f;
    buttonUI_->Draw();
}
