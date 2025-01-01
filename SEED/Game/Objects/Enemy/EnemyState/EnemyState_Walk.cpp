#include "EnemyState_Walk.h"
#include "Enemy/Enemy.h"
// 各ステートのヘッダーをインクルード
#include "EnemyState_Idle.h"
#include "EnemyState_Attack.h"
#include "EnemyState_Damaged.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
EnemyState_Walk::EnemyState_Walk(BaseCharacter* player){
    Initialize(player);
}

EnemyState_Walk::~EnemyState_Walk(){}

void EnemyState_Walk::Initialize(BaseCharacter* player){
    ICharacterState::Initialize(player);
    pCharacter_->SetAnimation("walk", true);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Walk::Update(){
    // ステート管理
    ManageState();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Walk::Draw(){}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Walk::ManageState(){

    // Enemy型にキャスト
    Enemy* pEnemy = dynamic_cast<Enemy*>(pCharacter_);

    // 攻撃を受けたらダメージ状態に遷移
    if(pEnemy->GetIsDamaged()){
        pCharacter_->ChangeState(new EnemyState_Damaged(pCharacter_));
        return;
    }

    // 攻撃可能距離にプレイヤーがいたら攻撃状態に遷移
    if(pEnemy->GetDistanceToPlayer() < attackRange_){
        pCharacter_->ChangeState(new EnemyState_Attack(pCharacter_));
        return;
    }

}
