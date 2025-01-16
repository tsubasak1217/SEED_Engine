#include "EnemyState_Chase.h"

// state
#include "EnemyState_RoutineMove.h"
// object
#include "Enemy/Enemy.h"

// lib
#include "JsonManager/JsonCoordinator.h"
#include "ClockManager.h"

EnemyState_Chase::EnemyState_Chase(const std::string& stateName,BaseCharacter* enemy){
    Initialize(stateName,enemy);
}

EnemyState_Chase::~EnemyState_Chase(){}

void EnemyState_Chase::Initialize(const std::string& stateName,BaseCharacter* enemy){
    enemy_ = dynamic_cast<Enemy*>(enemy);
    ICharacterState::Initialize(stateName,enemy_);
    JsonCoordinator::RegisterItem(enemy_->GetName(),"sensingDistance",sensingDistance_);
    JsonCoordinator::RegisterItem(enemy_->GetName(),"speed",speed_);
}

void EnemyState_Chase::Update(){
    if(!enemy_->GetTargetPlayer()){ return; }
    Vector3 moveVec = MyMath::Normalize(enemy_->GetTargetPlayer()->GetWorldTranslate() - enemy_->GetWorldTranslate());
    enemy_->SetTranslate(enemy_->GetWorldTranslate() + moveVec * speed_ * ClockManager::DeltaTime());
    ManageState();
}

void EnemyState_Chase::Draw(){}

void EnemyState_Chase::ManageState(){
    if(enemy_->GetDistanceToPlayer() > sensingDistance_){
        enemy_->ChangeState(new EnemyState_RoutineMove("EnemyState_RoutineMove",enemy_));
    }
}
