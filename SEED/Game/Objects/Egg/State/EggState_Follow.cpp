#include "EggState_Follow.h"

#include "MyMath.h"

EggState_Follow::EggState_Follow(BaseCharacter* _egg,BaseObject* _followTarget)
    :followTarget_(_followTarget){
    pCharacter_ = _egg;
}

EggState_Follow::~EggState_Follow(){}

void EggState_Follow::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);
}

void EggState_Follow::Update(){
    MoveFollow();
    ManageState();
}

void EggState_Follow::Draw(){}

void EggState_Follow::MoveFollow(){
    // プレイヤーの位置を取得
    Vector3 followTargetPos = followTarget_->GetWorldTranslate();
    // プレイヤーの位置にオフセットを加算
    followTargetPos += followOffset_ * RotateYMatrix(followTarget_->GetWorldRotate().y);
    // エッグの位置を取得
    Vector3 eggPos = pCharacter_->GetWorldTranslate();

    // プレイヤーの位置に向かって移動
    pCharacter_->SetTranslate(MyMath::Lerp(eggPos,followTargetPos,followSensitivity_));
}

void EggState_Follow::ManageState(){}
