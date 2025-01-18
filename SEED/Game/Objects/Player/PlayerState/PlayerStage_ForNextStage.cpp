#include "PlayerStage_ForNextStage.h"

//otherState
#include "PlayerState_Idle.h"
//eggState
#include "Egg/State/EggState_Thrown.h"

//object
#include "Egg/Egg.h"
#include "Player/Player.h"
//manager
#include "Egg/Manager/EggManager.h"

//etc
#include "MyFunc.h"

PlayerStage_ForNextStage::PlayerStage_ForNextStage(
    const Vector3& nextStartPos,
    BaseCharacter* player)
    :nextStartPos_(nextStartPos){
    Initialize("PlayerStage_ForNextStage",player);
}

PlayerStage_ForNextStage::~PlayerStage_ForNextStage(){}

void PlayerStage_ForNextStage::Initialize(
    const std::string& _stateName,
    BaseCharacter* _player){
    ICharacterState::Initialize(_stateName,_player);

    //EggManagerを取得するためにPlayerをダウンキャスト
    Player* player = dynamic_cast<Player*>(pCharacter_);
    EggManager* eggManager = player->GetEggManager();

    //卵がないなら生成
    if(eggManager->GetIsEmpty()){
        std::unique_ptr<Egg> newEgg = std::make_unique<Egg>(player);
        newEgg->Initialize();
        eggManager->AddEgg(newEgg);
    }
    egg_ = eggManager->GetFrontEgg().get();

    const Vector3& startPos = pCharacter_->GetWorldTranslate();
    Vector3 eggVelocity = MyFunc::CalculateInitialVelocity3D(startPos,nextStartPos_,-9.8f * egg_->GetWeight());
    
    egg_->SetVelocity(eggVelocity);
    egg_->ChangeState(new EggState_Thrown(egg_,eggVelocity,0.0f,MyMath::Length(eggVelocity)));

    // 地面から落ちないように 重力を適用しない
    pCharacter_->SetIsApplyGravity(false);
}

void PlayerStage_ForNextStage::Update(){
    //ステート管理
    ManageState();
}

void PlayerStage_ForNextStage::Draw(){}


void PlayerStage_ForNextStage::ManageState(){
    //ステートが終了したら
    if(!egg_){
        pCharacter_->SetIsApplyGravity(true);
        //操作可能にする
        pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
    }
}
