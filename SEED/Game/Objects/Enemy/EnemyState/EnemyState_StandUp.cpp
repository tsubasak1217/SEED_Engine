#include "EnemyState_StandUp.h"
// 各ステートのヘッダーをインクルード
#include "EnemyState_Idle.h"
#include "EnemyState_Damaged.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
EnemyState_StandUp::EnemyState_StandUp(const std::string& stateName, BaseCharacter* player){
    Initialize(stateName, player);
}

EnemyState_StandUp::~EnemyState_StandUp(){}

void EnemyState_StandUp::Initialize(const std::string& stateName, BaseCharacter* player){
    ICharacterState::Initialize(stateName, player);
    pCharacter_->SetAnimation("standUp", false);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_StandUp::Update(){
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_StandUp::Draw(){}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void EnemyState_StandUp::ManageState(){

    // 立ち上がり切ったらアイドル状態に遷移
    if(pCharacter_->GetIsEndAnimation()){
        pCharacter_->ChangeState(new EnemyState_Idle("Enemy_Idle", pCharacter_));
        return;
    }

    // ダメージを受けたらダメージ状態に遷移
    if(pCharacter_->GetIsDamaged()){
        pCharacter_->ChangeState(new EnemyState_Damaged("Enemy_Damaged",pCharacter_));
        return;
    }

}