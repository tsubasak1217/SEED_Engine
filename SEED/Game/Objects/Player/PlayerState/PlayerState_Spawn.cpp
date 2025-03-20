#include "PlayerState_Spawn.h"

//others state
#include "PlayerState_Idle.h"
//egg State
#include "Egg/State/EggState_Idle.h"
#include "Egg/State/EggState_Follow.h"

//player
#include "Egg/Egg.h"
#include "Player/Player.h"
#include "PlayerCorpse/PlayerCorpse.h"
// camera
#include "Camera/FollowCamera.h"
//manager
#include "ParticleManager.h"
#include "Egg/Manager/EggManager.h"
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"
#include "ClockManager.h"
#include "AudioManager.h"

/// math
#include "MyMath.h"

PlayerState_Spawn::PlayerState_Spawn(){}

PlayerState_Spawn::PlayerState_Spawn(const std::string& stateName,BaseCharacter* player,Egg* _egg,bool _spawnCorpse){
    egg_ = _egg;
    spawnCorpse_ = _spawnCorpse;
    Initialize(stateName,player);
}

PlayerState_Spawn::~PlayerState_Spawn(){
    // 重力を適応
    // 当たり判定を取るように
    pCharacter_->SetIsApplyGravity(true);
    pCharacter_->SetCollidable(true);
}

void PlayerState_Spawn::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    // Player
    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    {
        deadPos_ = pCharacter_->GetWorldTranslate();
        pPlayer->SetAnimation("dead",false);
        // 移動先 
        spawnPos_ = egg_->GetWorldTranslate();

        // 移動不可にする
        pPlayer->SetIsMovable(false);

        // 生前の情報を保存
        beforePlayerScale_ = pPlayer->GetWorldScale();
        beforePlayerWeight_ = pPlayer->GetSwitchPushWeight();

        // 重力を受けないように
        pCharacter_->SetIsJump(false);
        pCharacter_->SetIsDrop(true);
        pCharacter_->SetJumpPower(0.f);
    }

    // ghost
    {
        ghostObject_ = std::make_unique<BaseObject>("dinosaur_ghost.obj");
        ghostObject_->SetTranslate(deadPos_);

        ghostObject_->SetRotateWithQuaternion(false);
        const Vector3 diffP2Spawn = spawnPos_ - deadPos_;
        float ghostRotateY = atan2f(diffP2Spawn.x,diffP2Spawn.z);
        ghostObject_->SetRotate({0.f,ghostRotateY,0.f});
        ghostObject_->UpdateMatrix();

        FollowCamera* pFollowCamera = dynamic_cast<FollowCamera*>(pPlayer->GetFollowCamera());
        pFollowCamera->SetTarget(ghostObject_.get());
    }

    // egg
    {
        egg_->UpdateMatrix();
        Vector3 eggBeforeScale = egg_->GetLocalScale();
        Vector3 eggBeforeRotate = egg_->GetLocalRotate();
        Vector3 eggBeforeTranslate = egg_->GetLocalTranslate();
        const BaseObject* beforeParent = egg_->GetParent();

        egg_->ChangeModel("egg_born.gltf");

        egg_->SetScale(eggBeforeScale);
        egg_->SetRotate(eggBeforeRotate);
        egg_->SetTranslate(eggBeforeTranslate);
        egg_->UpdateMatrix();

        if(beforeParent){
            egg_->SetParent(beforeParent);
        }

        egg_->InitColliders(ObjectType::Egg);

        egg_->ChangeState(new EggState_Idle(egg_));

        /*
        投げる卵は 後ろから取得するため
        前方の卵と一致すれば
        それ以外の卵は存在しないことになる
        */
        if(egg_ != pPlayer->GetEggManager()->GetFrontEgg().get()){
            playerHasManyEggs_ = true;
            auto& frontEgg = pPlayer->GetEggManager()->GetFrontEgg();
            frontEgg->ChangeState(new EggState_Follow(frontEgg.get(),ghostObject_.get()));
        }
    }

    // effect
    ParticleManager::AddEffect("SoulTrajectory.json",{0.f,0.f,0.f},ghostObject_->GetWorldMatPtr());
}

void PlayerState_Spawn::Update(){

    pCharacter_->SetIsDrop(false);
    pCharacter_->SetDropSpeed(0.0f);

    if(!movedGhost_){
        elapsedTime_ += ClockManager::DeltaTime();
        elapsedTime_ = (std::min)(elapsedTime_,ghostMoveTime_);

        {// moving ghost
            const float t = std::clamp(elapsedTime_ / ghostMoveTime_,0.f,ghostMoveTime_);
            ghostObject_->SetTranslate(MyMath::Lerp(deadPos_,spawnPos_,t));
        }
        ghostObject_->Update();
    } else{
        elpsedAnimationTime_ += ClockManager::DeltaTime();
    }
}

void PlayerState_Spawn::Draw(){
    if(!movedGhost_){
        ghostObject_->Draw();
    }
}

void PlayerState_Spawn::ManageState(){
    if(movedGhost_){
        // ghostが移動し終わって Animation が終了していたら IdleStateへ
        if((std::max)(playerAnimationDuration_,eggAnimationDuration_) <= elpsedAnimationTime_){

            Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
            // カメラのターゲットをplayerに
            FollowCamera* pCamera = dynamic_cast<FollowCamera*>(pPlayer->GetFollowCamera());
            pCamera->SetTarget(pPlayer);

            // ここで,移動可能にする
            pCharacter_->SetIsMovable(true);

            // 重力を適応,当たり判定を取るように
            pCharacter_->SetIsApplyGravity(true);
            pCharacter_->SetCollidable(true);

            // たまごを消す
            egg_->Break();

            if(playerHasManyEggs_){
                auto& frontEgg = pPlayer->GetEggManager()->GetFrontEgg();
                frontEgg->ChangeState(new EggState_Follow(frontEgg.get(),pPlayer));
            }

            pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
        }
    } else{
        // ghostが移動し終わったら
        if(elapsedTime_ >= ghostMoveTime_){
            // アニメーションを流す
            egg_->SetAnimation("born",false);
            pCharacter_->SetAnimation("born",false);

            // アニメーションの時間を取得
            playerAnimationDuration_ = pCharacter_->GetAnimationDuration();
            eggAnimationDuration_ = egg_->GetAnimationDuration();
            elpsedAnimationTime_ = 0.f;

            //Sound
            AudioManager::PlayAudio("SE/dinosaur_born.wav",false,0.7f);

            // 死体を作成
            if(spawnCorpse_){
                Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
                std::unique_ptr<PlayerCorpse> pCorpse = std::make_unique<PlayerCorpse>();
                pCorpse->Initialize();
                // Playerが parent を持っていれば parentを 継承
                if(pCharacter_->GetParent()){
                    pCorpse->SetParent(pCharacter_->GetParent());
                }
                pCorpse->SetManager(pPlayer->GetCorpseManager());
                pCorpse->SetScale(beforePlayerScale_);
                pCorpse->SetSwitchPushWeight(beforePlayerWeight_);
                pCorpse->SetRotateX(pPlayer->GetWorldRotate().x);
                pCorpse->SetRotateY(pPlayer->GetWorldRotate().y);
                pCorpse->SetTranslate(pCharacter_->GetLocalTranslate());
                pPlayer->GetCorpseManager()->AddPlayerCorpse(pCorpse);
            }

            // 移動
            // 卵が親子付けされていたらplayerも親子付け
            // Playerの親子付けを解除する前に保存
            pCharacter_->ReleaseParent();// 親子付けを解除
            if(egg_->GetParent()){
                Matrix4x4 invParentMat = InverseMatrix(egg_->GetParent()->GetWorldMat());
                Vector3 localTranslate = spawnPos_ * invParentMat;
                localTranslate *= ExtractScale(egg_->GetParent()->GetWorldMat());
                pCharacter_->SetTranslate(localTranslate);
                pCharacter_->SetParent(egg_->GetParent());
            } else{
                pCharacter_->SetTranslate(spawnPos_);
            }

            pCharacter_->SetTranslateY(pCharacter_->GetLocalTranslate().y + 0.2f);

            pCharacter_->UpdateMatrix();
            for(auto& collider : pCharacter_->GetColliders()){
                collider->Update();
            }

            // 重力を適応,当たり判定を取らないように
            pCharacter_->SetIsApplyGravity(false);
            pCharacter_->SetCollidable(false);

            movedGhost_ = true;
        }
    }
}
