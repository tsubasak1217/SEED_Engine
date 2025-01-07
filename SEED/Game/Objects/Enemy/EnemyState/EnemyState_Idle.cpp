#include "EnemyState_Idle.h"
#include "Enemy/Enemy.h"
// 状態クラスのインクルード
#include "EnemyState_Damaged.h"
#include "EnemyState_Walk.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
EnemyState_Idle::EnemyState_Idle(const std::string& stateName, BaseCharacter* enemy){
    Initialize(stateName, enemy);
}

EnemyState_Idle::~EnemyState_Idle(){}

void EnemyState_Idle::Initialize(const std::string& stateName, BaseCharacter* enemy){
    ICharacterState::Initialize(stateName, enemy);
    pCharacter_->HandleRotate(Vector3(0.0f, 3.14f, 0.0f));
    pCharacter_->SetAnimation("idle", true);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Idle::Update(){
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Idle::Draw(){}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Idle::ManageState(){

    // Enemy型にキャスト
    Enemy* pEnemy = dynamic_cast<Enemy*>(pCharacter_);

    // ダメージを受けたらダメージ状態に遷移
    if(pEnemy->GetIsDamaged()){
        pCharacter_->ChangeState(new EnemyState_Damaged("Enemy_Damaged", pCharacter_));
        return;
    }

    // プレイヤーを発見したら追跡状態に遷移
    if(pEnemy->GetDistanceToPlayer() < sensingDistance_){
        pCharacter_->ChangeState(new EnemyState_Walk("Enemy_Walk",pCharacter_));
        return;
    }
}
