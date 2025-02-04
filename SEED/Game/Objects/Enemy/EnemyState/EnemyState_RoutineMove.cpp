#include "EnemyState_RoutineMove.h"

// state
#include "EnemyState_Chase.h"

// object
#include "Enemy/Enemy.h"
// manager
#include "EnemyManager.h"
#include "ClockManager.h"
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
    sensingDistance_ = 15.0f;
    //ルーチンポイントの取得
    movePoints_ = &enemy_->GetRoutinePoints();
}

void EnemyState_RoutineMove::Update(){

    if (!movePoints_ || movePoints_->size() < 1){ return; }

    Vector3 currentPos = enemy_->GetWorldTranslate();
    Vector3 targetPos = (*movePoints_)[currentMovePointIndex_];
    Vector3 direction = targetPos - currentPos;
    float distanceToTarget = MyMath::Length(direction);

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

    float yaw = std::atan2(moveDir.x, moveDir.z);
    float pitch = std::atan(moveDir.y);
    float roll = 0.0f;

    Vector3 targetEuler = Vector3(pitch, yaw, roll);

    // 現在のEuler角との補間
    Vector3 currentEuler = enemy_->GetWorldRotate();
    Vector3 newEuler = MyMath::Lerp(currentEuler, targetEuler, 2.0f * ClockManager::DeltaTime());

    enemy_->SetRotate(newEuler);


    // 移動後に距離を再チェック
    currentPos = enemy_->GetWorldTranslate();
    targetPos = (*movePoints_)[currentMovePointIndex_];
    distanceToTarget = MyMath::Length(currentPos, targetPos);

    if (distanceToTarget < 0.01f){
        currentMovePointIndex_ = (currentMovePointIndex_ + 1) % movePoints_->size();
    }
}



void EnemyState_RoutineMove::Draw(){}

void EnemyState_RoutineMove::EndFrame(){
    ICharacterState::EndFrame();
}

void EnemyState_RoutineMove::ManageState(){
    if(enemy_->GetDistanceToPlayer() < sensingDistance_){
        if(!enemy_->GetChasePlayer()){
            return;
        }
        enemy_->ChangeState(new EnemyState_Chase("Enemy_Chase",enemy_));
    }
}
