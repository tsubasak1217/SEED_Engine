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
    JsonCoordinator::RegisterItem(enemy_->GetName(),"speed",speed_);
    //ルーチンポイントの取得
    movePoints_ = &enemy_->GetRoutinePoints();
    speed_ = 1.0f;
}

void EnemyState_RoutineMove::Update(){
    if(!movePoints_||movePoints_->size()<1){ return; }
    // ルーチンポイントを巡回
    if(MyMath::Length(enemy_->GetWorldTranslate(),(*movePoints_)[currentMovePointIndex_]) < 0.001f){
        currentMovePointIndex_++;
        currentMovePointIndex_ = currentMovePointIndex_ % movePoints_->size();
    }
    // ルーチンポイントに向かって移動
    Vector3 moveVec = MyMath::Normalize((*movePoints_)[currentMovePointIndex_] - enemy_->GetWorldTranslate());
    enemy_->SetTranslate(enemy_->GetWorldTranslate() + moveVec * speed_ * ClockManager::DeltaTime());
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
