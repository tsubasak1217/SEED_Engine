#include "EnemyState_RoutineMove.h"

// state
#include "EnemyState_Chase.h"

// object
#include "Enemy/Enemy.h"
// manager
#include "EnemyManager.h"

// lib
#include "../adapter/json/JsonCoordinator.h"
#include "ClockManager.h"

EnemyState_RoutineMove::EnemyState_RoutineMove(const std::string& stateName,BaseCharacter* enemy){
    Initialize(stateName,enemy);
}

EnemyState_RoutineMove::~EnemyState_RoutineMove(){}

void EnemyState_RoutineMove::Initialize(const std::string& stateName,BaseCharacter* enemy){
    enemy_ = dynamic_cast<Enemy*>(enemy);
    ICharacterState::Initialize(stateName,enemy_);

    JsonCoordinator::RegisterItem(enemy_->GetName(),"sensingDistance",sensingDistance_);
    //ルーチンポイントの取得
    movePoints_ = &enemy_->GetRoutinePoints();
}

void EnemyState_RoutineMove::Update(){
    if (!movePoints_ || movePoints_->size() < 1){ return; }

    Vector3 currentPos = enemy_->GetWorldTranslate();
    Vector3 targetPos = (*movePoints_)[currentMovePointIndex_];
    Vector3 direction = targetPos - currentPos;
    float distanceToTarget = MyMath::Length(direction);

    if (distanceToTarget < 0.001f){
        // ターゲットポイントにほぼ到達したと判断
        currentMovePointIndex_ = (currentMovePointIndex_ + 1) % movePoints_->size();
        return; // 次のフレームで新しいポイントに向かって移動
    }

    // 正規化された移動方向
    Vector3 moveDir = MyMath::Normalize(direction);
    // 1フレームあたりの移動量
    float moveStep = enemy_->GetMoveSpeed() * ClockManager::DeltaTime();

    if (moveStep >= distanceToTarget){
        // 移動ステップがターゲットまでの距離以上の場合、ターゲットに直接移動
        enemy_->SetTranslate(targetPos);
        currentMovePointIndex_ = (currentMovePointIndex_ + 1) % movePoints_->size();
    } else{
        // 通常通り移動
        enemy_->SetTranslate(currentPos + moveDir * moveStep);
    }

    ManageState();
}


void EnemyState_RoutineMove::Draw(){}

void EnemyState_RoutineMove::ManageState(){
    if(enemy_->GetDistanceToPlayer() < sensingDistance_){
        if(!enemy_->GetChasePlayer()){
            return;
        }
        enemy_->ChangeState(new EnemyState_Chase("Enemy_Chase",enemy_));
    }
}
