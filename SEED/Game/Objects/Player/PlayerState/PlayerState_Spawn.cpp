#include "PlayerState_Spawn.h"

//others state
#include "PlayerState_Idle.h"

//player
#include "Player/Player.h"
#include "PlayerCorpse/PlayerCorpse.h"
//manager
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"

//manager
#include "ClockManager.h"

PlayerState_Spawn::PlayerState_Spawn(){}

PlayerState_Spawn::PlayerState_Spawn(const std::string& stateName,BaseCharacter* player){
    Initialize(stateName,player);
}

PlayerState_Spawn::~PlayerState_Spawn(){}

void PlayerState_Spawn::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    pCharacter_->SetAnimation("born",false);
}

void PlayerState_Spawn::Update(){
    ManageState();
}

void PlayerState_Spawn::Draw(){}

void PlayerState_Spawn::ManageState(){
    if(pCharacter_->GetIsEndAnimation()){
        Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
        std::unique_ptr<PlayerCorpse> pCorpse = std::make_unique<PlayerCorpse>();
        pCorpse->Initialize();
        pCorpse->SetManager(pPlayer->GetCorpseManager());
        pCorpse->SetTranslate(pCharacter_->GetWorldTranslate());
        pPlayer->GetCorpseManager()->AddPlayerCorpse(pCorpse);

        // 移動
        pCharacter_->SetTranslate(spawnPos_);
        pPlayer->SetPrePos(spawnPos_);
        pCharacter_->DiscardPreCollider();
        pPlayer->UpdateMatrix();

        pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
    }
}
