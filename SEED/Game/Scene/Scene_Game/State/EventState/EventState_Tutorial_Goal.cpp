#include "EventState_Tutorial_Goal.h"
#include "Scene_Game.h"

//lib
#include "../PlayerInput/PlayerInput.h"

EventState_Tutorial_Goal::EventState_Tutorial_Goal(){
}

EventState_Tutorial_Goal::EventState_Tutorial_Goal(Scene_Base* pScene)
    : EventState_Tutorial_Base(pScene){

    // ゴールに注目し、補間を遅く設定
    pCamera_->SetTarget(pGameScene_->Get_pStageManager()->GetCurrentStage()->GetGoalObject());
    pCamera_->SetInterpolationRate(0.05f);

    // テキストのステップ数を設定
    textStepMax_ = 3;
    textSprite_->clipLT.y = spriteClipHeightOffset_;
}

EventState_Tutorial_Goal::~EventState_Tutorial_Goal(){
    pPlayer_->SetIsMovable(true);
    pCamera_->SetTarget(pPlayer_);
    pCamera_->SetInterpolationRate(0.075f);
}


void EventState_Tutorial_Goal::Update(){
    EventState_Tutorial_Base::Update();
}


void EventState_Tutorial_Goal::Draw(){
    EventState_Tutorial_Base::Draw();
}
