#include "PlayerState_Eat.h"

// others State
#include "PlayerState_Idle.h"
///stl
#include "algorithm"
///local
//object
#include "Enemy/Enemy.h"
#include "Egg/Egg.h"
// manager
#include "ClockManager.h"
#include "Egg/Manager/EggManager.h"
#include "Player/PredationRange/PredationRange.h"
//math
#include "MatrixFunc.h"
#include "MyMath.h"

PlayerState_Eat::PlayerState_Eat(BaseCharacter* player){
    Initialize("PlayerState_Eat", player);
}

PlayerState_Eat::~PlayerState_Eat(){}

void PlayerState_Eat::Update(){
    currentTime_ += ClockManager::DeltaTime();

    currentUpdate_();
}

void PlayerState_Eat::Draw(){}

void PlayerState_Eat::Initialize(const std::string& stateName, BaseCharacter* player){
    ICharacterState::Initialize(stateName, player);

    { // 捕食対象の取得
        Player* pPlayer = dynamic_cast< Player* >(pCharacter_);
        // 捕食対象リストの先頭を取得(一番近い敵)
        enemy_ = pPlayer->GetPredationRange()->GetPreyList().front().enemy;
    }

    {
        // 捕食対象の方向を向く
        Vector3 direction = enemy_->GetWorldTranslate() - pCharacter_->GetWorldTranslate();
        interpolationRotateY_ = atan2f(direction.x, direction.z);

    }

    currentUpdate_ = [this] (){RotateForEnemy(); };
}

void PlayerState_Eat::ManageState(){}

void PlayerState_Eat::RotateForEnemy(){
    float t = currentTime_ / rotateTime_;
    float interpolatedRotateY = MyMath::LerpShortAngle(pCharacter_->GetLocalRotate().y, interpolationRotateY_, std::clamp(t, 0.f, 1.f));
    pCharacter_->SetRotateY(interpolatedRotateY);

    if (t >= 1.0f){
        currentTime_ = 0.f;
        currentUpdate_ = [this] (){EatEnemy(); };
    }
}

void PlayerState_Eat::EatEnemy(){
    float t = currentTime_ / eatTime_;
    // なんかアニメーションが流れる
    if (t >= 1.0f){
        currentTime_ = 0.f;
        currentUpdate_ = [this] (){SpawnEgg(); };

        // 敵を削除
        enemy_->SetIsAlive(false);
        // ここ 以降で enemy_ にアクセスしないように
        enemy_ = nullptr;
    }
}

void PlayerState_Eat::SpawnEgg(){
    float t = currentTime_ / spawnEggTime_;
    // なんかアニメーションが流れる

    if (t >= 1.0f){
        Player* pPlayer = dynamic_cast< Player* >(pCharacter_);
        // 卵を生成
        {
            std::unique_ptr<Egg> spawnedEgg = std::make_unique<Egg>(pPlayer);
            spawnedEgg->SetTranslate(pPlayer->GetWorldTranslate());
            pPlayer->GetEggManager()->AddEgg(spawnedEgg);
        }

        // ステートを変更
        pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle", pCharacter_));
    }
}
