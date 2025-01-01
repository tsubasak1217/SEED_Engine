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
// 移動処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Walk::Move(){

    // Enemy型にキャスト
    Enemy* pEnemy = dynamic_cast<Enemy*>(pCharacter_);
    // プレイヤーへの方向を計算
    Vector3 moveDirection = 
        pEnemy->GetTargetPlayer()->GetWorldTranslate() - pEnemy->GetWorldTranslate();
    moveDirection.y = 0.0f;
    moveDirection = MyMath::Normalize(moveDirection);

    // 移動
    pEnemy->HandleMove(moveDirection * moveSpeed_ * ClockManager::DeltaTime());
    pEnemy->UpdateMatrix();
}

//////////////////////////////////////////////////////////////////////////
// 移動に合わせた回転処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Walk::Rotate(){

    // 移動ベクトルを求める
    Vector3 moveVec = pCharacter_->GetWorldTranslate() - pCharacter_->GetPrePos();

    // 移動ベクトルから回転を求める
    if(MyMath::Length(moveVec) > 0.0f){
        Vector3 rotateVec_ = MyFunc::CalcRotateVec(moveVec);
        rotateVec_.x = 0.0f;

        if(isLerpRotate_){// 補間回転する場合

            // 補間後の回転を求める
            Vector3 lerped = Quaternion::ToEuler(
                Quaternion::Slerp(
                    pCharacter_->GetWorldRotate(),
                    rotateVec_,
                    lerpRate_ * ClockManager::TimeRate()
                )
            );

            pCharacter_->HandleRotate(lerped);

        } else{
            pCharacter_->HandleRotate(rotateVec_);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void EnemyState_Walk::Update(){

    // 移動処理
    Move();

    // 回転処理
    Rotate();

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
