#include "EnemyState_Down.h"
// 各ステートのヘッダーをインクルード
#include "EnemyState_StandUp.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
EnemyState_Down::EnemyState_Down(const std::string& stateName, BaseCharacter* player){
    Initialize(stateName, player);
    // 無敵時間の設定
    pCharacter_->SetUnrivalledTime(pCharacter_->GetAnimationDuration());
}

EnemyState_Down::~EnemyState_Down(){}

void EnemyState_Down::Initialize(const std::string& stateName, BaseCharacter* player){
    ICharacterState::Initialize(stateName, player);
    pCharacter_->SetAnimation("down", false);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Down::Update(){

}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Down::Draw(){}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Down::ManageState(){

    // 時間がたったら立ち上がる
    if(pCharacter_->GetIsEndAnimation()){
        if(pCharacter_->GetHP() > 0){
            // 生きてたら立ち上がる
            pCharacter_->ChangeState(new EnemyState_StandUp("Enemy_StandUp", pCharacter_));
        } else{
            // 死んでたら死亡フラグを立てる
            pCharacter_->SetIsAlive(false);
        }
        return;
    }

}