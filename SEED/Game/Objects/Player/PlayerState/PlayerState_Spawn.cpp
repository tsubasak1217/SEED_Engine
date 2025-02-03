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
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"
#include "ClockManager.h"

PlayerState_Spawn::PlayerState_Spawn(){}

PlayerState_Spawn::PlayerState_Spawn(const std::string& stateName,BaseCharacter* player,Egg* _egg){
    egg_ = _egg;
    Initialize(stateName,player);
}

PlayerState_Spawn::~PlayerState_Spawn(){}

void PlayerState_Spawn::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    // 死体を作成
    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    std::unique_ptr<PlayerCorpse> pCorpse = std::make_unique<PlayerCorpse>();
    pCorpse->Initialize();
    pCorpse->SetManager(pPlayer->GetCorpseManager());
    pCorpse->SetTranslate(pCharacter_->GetWorldTranslate());
    pPlayer->GetCorpseManager()->AddPlayerCorpse(pCorpse);

    // とりあえず,ここで 移動
    spawnPos_ = egg_->GetWorldTranslate();
    pCharacter_->ReleaseParent();// 親子付けを解除
    pCharacter_->SetTranslate(spawnPos_);

    pCharacter_->DiscardPreCollider();
    pPlayer->UpdateMatrix();
    // 移動不可にする
    pPlayer->SetIsMovable(false);

    // アニメーションの設定
    pCharacter_->SetAnimation("born",false);

    {
        Vector3 eggBeforeScale = egg_->GetLocalScale();
        Vector3 eggBeforeRotate = egg_->GetLocalRotate();
        Vector3 eggBeforeTranslate = egg_->GetLocalTranslate();

        egg_->ChangeModel("egg_born.gltf");

        egg_->SetScale(eggBeforeScale);
        egg_->SetRotate(eggBeforeRotate);
        egg_->SetTranslate(eggBeforeTranslate);

        egg_->SetAnimation("born",false);
        egg_->InitColliders(ObjectType::Egg);

        egg_->ChangeState(new EggState_Idle(egg_));
    }

    // カメラのターゲットをplayerに
    FollowCamera* pCamera = dynamic_cast<FollowCamera*>(pPlayer->GetFollowCamera());
    pCamera->SetTarget(pPlayer);

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

void PlayerState_Spawn::Update(){}

void PlayerState_Spawn::Draw(){}

void PlayerState_Spawn::ManageState(){
    if(pCharacter_->GetIsEndAnimation() && egg_->GetIsEndAnimation()){

        // ここで,移動可能にする
        pCharacter_->SetIsMovable(true);

        // たまごを消す
        egg_->Break();
        pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
    }
}
