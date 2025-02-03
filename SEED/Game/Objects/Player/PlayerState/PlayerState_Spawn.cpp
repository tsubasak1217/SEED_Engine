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

    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    // とりあえず,ここで 移動
    spawnPos_ = egg_->GetWorldTranslate();
    pCharacter_->ReleaseParent();// 親子付けを解除
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

    // effect
    ParticleManager::AddEffect("SoulTrajectory.json",{0.f,0.f,0.f},ghostObject_->GetWorldMatPtr());
  
    // 卵が親子付けされていたらplayerも親子付け
    if(egg_->GetParent()){
        Vector3 preTranslate = pCharacter_->GetWorldTranslate();
        Matrix4x4 invParentMat = InverseMatrix(egg_->GetParent()->GetWorldMat());
        Vector3 localTranslate = preTranslate * invParentMat;
        localTranslate *= ExtractScale(egg_->GetParent()->GetWorldMat());
        pCharacter_->SetTranslate(localTranslate);
        pCharacter_->SetParent(egg_->GetParent());
        pCharacter_->UpdateMatrix();
    }
}

void PlayerState_Spawn::Update(){
    if(!movedGhost_){
        elapsedTime_ += ClockManager::DeltaTime();
        elapsedTime_ = (std::min)(elapsedTime_,ghostMoveTime_);

        {// moving ghost
            const float t = elapsedTime_ / ghostMoveTime_;
            ghostObject_->SetTranslate(MyMath::Lerp(deadPos_,spawnPos_,t));
        }
        ghostObject_->Update();
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

            Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
            // カメラのターゲットをplayerに
            FollowCamera* pCamera = dynamic_cast<FollowCamera*>(pPlayer->GetFollowCamera());
            pCamera->SetTarget(pPlayer);
            // ここで,移動可能にする
            pCharacter_->SetIsMovable(true);

            // たまごを消す
            egg_->Break();
            pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
        }
    } else{
        if(elapsedTime_ >= ghostMoveTime_){
            // アニメーションを流す
            egg_->SetAnimation("born",false);
            pCharacter_->SetAnimation("born",false);

            // 死体を作成
            Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
            std::unique_ptr<PlayerCorpse> pCorpse = std::make_unique<PlayerCorpse>();
            pCorpse->Initialize();
            pCorpse->SetManager(pPlayer->GetCorpseManager());
            pCorpse->SetTranslate(deadPos_);
            pPlayer->GetCorpseManager()->AddPlayerCorpse(pCorpse);

            movedGhost_ = true;
        }
    }
}
