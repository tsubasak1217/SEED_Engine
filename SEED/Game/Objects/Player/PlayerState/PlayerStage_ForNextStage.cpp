#include "PlayerStage_ForNextStage.h"

// otherState
#include "PlayerState_Idle.h"
// eggState
#include "Egg/State/EggState_Break.h"
#include "Egg/State/EggState_Idle.h"

// object
#include "Egg/Egg.h"
#include "Player/Player.h"
//camera
#include "FollowCamera.h"
// manager
#include "Egg/Manager/EggManager.h"
#include "StageManager.h"
//engine 
#include "SEED.h"

// math & misc
#include "MyMath.h"
#include "MyFunc.h"

PlayerStage_ForNextStage::PlayerStage_ForNextStage(
    const Vector3& nextStartPos,
    BaseCharacter* player)
    : nextStartPos_(nextStartPos){
    Initialize("PlayerStage_ForNextStage",player);
}

PlayerStage_ForNextStage::~PlayerStage_ForNextStage(){}

void PlayerStage_ForNextStage::Initialize(
    const std::string& _stateName,
    BaseCharacter* _player){
    ICharacterState::Initialize(_stateName,_player);

    moveTime_ = 0.0f;

    // EggManagerを取得するためにPlayerをダウンキャスト
    Player* player = dynamic_cast<Player*>(pCharacter_);
    EggManager* eggManager = player->GetEggManager();

    // 卵がないなら生成
    if(eggManager->GetIsEmpty()){
        std::unique_ptr<Egg> newEgg = std::make_unique<Egg>(player);
        newEgg->Initialize();
        eggManager->AddEgg(newEgg);
    }
    egg_ = eggManager->GetFrontEgg().get();

    // このステートで完全に位置を操作する
    egg_->ChangeState(new EggState_Idle(egg_));

    beforePos_ = pCharacter_->GetWorldTranslate();

    Vector3 diff = nextStartPos_ - beforePos_;
    float heigherPoint = (std::max)(nextStartPos_.y,beforePos_.y);
    ctlPoint_ = beforePos_ + diff * 0.25f;
    ctlPoint_.y = heigherPoint + 20.0f;

    // カメラのターゲットを卵に変更
    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    FollowCamera* pFollowCamera =  dynamic_cast<FollowCamera*>(pPlayer->GetFollowCamera());
    pFollowCamera->SetTarget(egg_);

}

void PlayerStage_ForNextStage::Update(){
    preIsThrow_ = isThrow_;

    if(!isThrow_){
        moveTime_ += ClockManager::DeltaTime();
        float t = moveTime_ / moveTimeMax_;
        // 卵を投げる

        egg_->SetTranslate(
            MyMath::Bezier(
            beforePos_,
            ctlPoint_,
            nextStartPos_,
            t
        ));
        isThrow_  = t >= 1.0f;
    } 

}

void PlayerStage_ForNextStage::Draw(){
    float index = 0.0f;
    Vector3 prePos = beforePos_;
    for(index = 1; index < 32; index++){
        float t = index / 31.0f;
        Vector3 pos = MyMath::Bezier(
            beforePos_,
            ctlPoint_,
            nextStartPos_,
            t
        );
        SEED::DrawLine(prePos,pos,Vector4(0.3f,1.0f,0.3f,1.0f));
        prePos = pos;
    }
}

void PlayerStage_ForNextStage::ManageState(){
    if(!preIsThrow_ && isThrow_){
        egg_->SetTranslate(nextStartPos_);
        egg_->ChangeState(new EggState_Break(egg_,true));
    }
}
