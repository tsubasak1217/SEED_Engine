#include "PlayerState_Spawn.h"

//others state
#include "PlayerState_Idle.h"
//egg State
#include "Egg/State/EggState_Idle.h"

//player
#include "Egg/Egg.h"
#include "Player/Player.h"
#include "PlayerCorpse/PlayerCorpse.h"
// camera
#include "Camera/FollowCamera.h"
//manager
#include "ParticleManager.h"
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"
#include "ClockManager.h"

/// math
#include "MyMath.h"

PlayerState_Spawn::PlayerState_Spawn(){}

PlayerState_Spawn::PlayerState_Spawn(const std::string& stateName,BaseCharacter* player,Egg* _egg){
    egg_ = _egg;
    Initialize(stateName,player);
}

PlayerState_Spawn::~PlayerState_Spawn(){}

void PlayerState_Spawn::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    deadPos_ = pCharacter_->GetWorldTranslate();

    // 死体を作成
    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    std::unique_ptr<PlayerCorpse> pCorpse = std::make_unique<PlayerCorpse>();
    pCorpse->Initialize();
    pCorpse->SetManager(pPlayer->GetCorpseManager());
    pCorpse->SetTranslate(deadPos_);
    pPlayer->GetCorpseManager()->AddPlayerCorpse(pCorpse);

    // とりあえず,ここで 移動
    spawnPos_ = egg_->GetWorldTranslate();
    pCharacter_->SetTranslate(spawnPos_);

    pCharacter_->DiscardPreCollider();
    pPlayer->UpdateMatrix();
    // 移動不可にする
    pPlayer->SetIsMovable(false);

    // egg
    {
        Vector3 eggBeforeScale = egg_->GetLocalScale();
        Vector3 eggBeforeRotate = egg_->GetLocalRotate();
        Vector3 eggBeforeTranslate = egg_->GetLocalTranslate();

        egg_->ChangeModel("egg_born.gltf");

        egg_->SetScale(eggBeforeScale);
        egg_->SetRotate(eggBeforeRotate);
        egg_->SetTranslate(eggBeforeTranslate);

        egg_->InitColliders(ObjectType::Egg);

        egg_->ChangeState(new EggState_Idle(egg_));
    }

    // ghost
    {
        ghostObject_ = std::make_unique<BaseObject>("dinosaur_ghost.gltf");
        ghostObject_->Initialize();
        ghostObject_->SetTranslate(deadPos_);

        ghostObject_->SetRotateWithQuaternion(false);
        const Vector3 diffP2Spawn = spawnPos_ - deadPos_;
        float ghostRotateY = atan2f(diffP2Spawn.x,diffP2Spawn.z);
        float ghostRotateX = atan2f(diffP2Spawn.z,diffP2Spawn.y);
        ghostObject_->SetRotate({ghostRotateX,ghostRotateY,0.f});

        FollowCamera* pFollowCamera = dynamic_cast<FollowCamera*>(pPlayer->GetFollowCamera());
        pFollowCamera->SetTarget(ghostObject_.get());
    }

    // effect
    ParticleManager::AddEffect("SoulTrajectory.json",{0.f,0.f,0.f},pCharacter_->GetWorldMatPtr());

    // カメラのターゲットをplayerに
    FollowCamera* pCamera = dynamic_cast<FollowCamera*>(pPlayer->GetFollowCamera());
    pCamera->SetTarget(pPlayer);
}

void PlayerState_Spawn::Update(){
    if(!movedGhost_){
        elapsedTime_ += ClockManager::DeltaTime();
        elapsedTime_ = (std::min)(elapsedTime_,ghostMoveTime_);

        {// moving ghost
            const float t = elapsedTime_ / ghostMoveTime_;
            ghostObject_->SetTranslate(MyMath::Lerp(deadPos_,spawnPos_,t));
        }
    }

}

void PlayerState_Spawn::Draw(){
    if(!movedGhost_){
        ghostObject_->Draw();
    }
}

void PlayerState_Spawn::ManageState(){
    if(movedGhost_){
        if(pCharacter_->GetIsEndAnimation() && egg_->GetIsEndAnimation()){

            // ここで,移動可能にする
            pCharacter_->SetIsMovable(true);

            // たまごを消す
            egg_->Break();
            pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
        }
    } else{
        if(elapsedTime_ >= elapsedTime_){
            // アニメーションを流す
            egg_->SetAnimation("born",false);
            pCharacter_->SetAnimation("born",false);

            movedGhost_ = true;
        }
    }
}
