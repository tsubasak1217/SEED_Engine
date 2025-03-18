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
#include "AudioManager.h"
//lib
#include "../PlayerInput/PlayerInput.h"

//math
#include "MatrixFunc.h"
#include "MyMath.h"

PlayerState_Eat::PlayerState_Eat(BaseCharacter* player){
    Initialize("PlayerState_Eat",player);
}

PlayerState_Eat::~PlayerState_Eat(){}

void PlayerState_Eat::Update(){
    currentTime_ += ClockManager::DeltaTime();

    currentUpdate_();
}

void PlayerState_Eat::Draw(){}

void PlayerState_Eat::Initialize(const std::string& stateName,BaseCharacter* player){
    ICharacterState::Initialize(stateName,player);

    { // 捕食対象の取得
        Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
        // 捕食対象リストの先頭を取得(一番近い敵)
        enemy_ = pPlayer->GetPredationRange()->GetPreyList().front().enemy;
    }

    {
        // 捕食対象の方向を向く
        Vector3 direction = enemy_->GetWorldTranslate() - pCharacter_->GetWorldTranslate();
        Vector2 directionXZ = Vector2(direction.x,direction.z);
        interpolationRotateY_ = atan2f(directionXZ.x,directionXZ.y);
        interpolationRotateY_ = interpolationRotateY_;
    }

    currentUpdate_ = [this](){RotateForEnemy(); };
}

void PlayerState_Eat::ManageState(){}

void PlayerState_Eat::RotateForEnemy(){
    float t = currentTime_ / rotateTime_;
    t = std::clamp(t,0.f,1.f);
    // float interpolatedRotateY = MyMath::LerpShortAngle(pCharacter_->GetWorldRotate().y,interpolationRotateY_,t);
    pCharacter_->SetRotateY(interpolationRotateY_);

    if(t >= 1.0f){
        currentTime_ = 0.f;
        // アニメーションを流す
        pCharacter_->SetAnimation("eat",false);
        //Sound
        AudioManager::PlayAudio("SE/dinosaur_eat.wav",false,0.8f);

        currentUpdate_ = [this](){EatEnemy(); };
    }
}

void PlayerState_Eat::EatEnemy(){
    // なんかアニメーションが流れる
    if(pCharacter_->GetIsEndAnimation()){
        currentTime_ = 0.f;
        pCharacter_->SetAnimation("swallow",false);

        // 敵を削除
        enemy_->SetIsAlive(false);
        // ここ 以降で enemy_ にアクセスしないように
        enemy_ = nullptr;

        Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
        // プレイヤーの成長レベルを上げる
        pPlayer->StepGrowLevel(1);
        //Sound
        AudioManager::PlayAudio("SE/dinosaur_grow.wav",false,0.6f);

        currentUpdate_ = [this](){SpawnEgg(); };
        return;
    }
}

void PlayerState_Eat::SpawnEgg(){
    if(pCharacter_->GetIsEndAnimation()){
        Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
        // 卵を生成
        {
            std::unique_ptr<Egg> spawnedEgg = std::make_unique<Egg>(pPlayer);
            spawnedEgg->SetEggManager(pPlayer->GetEggManager());
            spawnedEgg->Initialize();
            spawnedEgg->SetTranslate(pPlayer->GetWorldTranslate());
            pPlayer->GetEggManager()->AddEgg(spawnedEgg);
        }

        // ステートを変更
        pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
    }
}
