#include "PlayerStage_ForNextStage.h"

// otherState
#include "PlayerState_Idle.h"
// eggState
#include "Egg/State/EggState_Break.h"
#include "Egg/State/EggState_Idle.h"

// object
#include "Egg/Egg.h"
#include "Player/Player.h"
// manager
#include "Egg/Manager/EggManager.h"

//engine 
#include "SEED.h"

// math & misc
#include "MyMath.h"
#include "MyFunc.h"

float CalclateParabolicInitialDirection(
    float _xLength,
    float _yHeight,
    float _gravity,
    float _initialSpeed){
    // 参考サイト
    // https://bibunsekibun.wordpress.com/2015/04/16/%E6%94%BE%E7%89%A9%E7%B7%9A%E3%81%A7%E7%9B%AE%E6%A8%99%E3%81%AB%E5%BD%93%E3%81%A6%E3%82%8B%E8%A7%92%E5%BA%A6%E3%82%92%E6%B1%82%E3%82%81%E3%82%8B/

    // 届くかどうかを調べる
    // tan^2θ - x/p tanθ + y/p + 1 = 0
    // から tanθ について 二次方程式を解く
    // a = 1
    float b = -1.0f * (2.0f * _initialSpeed * _initialSpeed * _xLength) / (_gravity * _xLength * _xLength);
    float c = 1.0f * (2.0f * _initialSpeed * _initialSpeed * _yHeight) / (_gravity * _xLength * _xLength);

    float D = b * b - 4 * c;
    // 届く
    if(D >= 0.0f){
        float sprtD = std::sqrt(D);
        float tanTheta1 = (-1.0f * b + sprtD) * 0.5f;
        float tanTheta2 = (-1.0f * b - std::sqrt(D)) * 0.5f;

        float theta = (std::min)(tanTheta1,tanTheta2);

        // 放物線は XY の平面上にあるので、Z軸回転は不要
        // そして 放物線は YZ平面で 使用するので x軸回転 に Thetaを適応
        return theta;
    }

    // 届かない
    return 0.0f;
}

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
    // ステート管理
    ManageState();

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
        egg_->ChangeState(new EggState_Break(egg_));
    }
    // ステートが終了したら
    if(!egg_){
        // 操作可能にする
        pCharacter_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pCharacter_));
    }
}
