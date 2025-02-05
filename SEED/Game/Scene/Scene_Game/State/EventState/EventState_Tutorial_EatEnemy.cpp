#include "EventState_Tutorial_EatEnemy.h"

//lib
#include "../PlayerInput/PlayerInput.h"

EventState_Tutorial_EatEnemy::EventState_Tutorial_EatEnemy(){
}

EventState_Tutorial_EatEnemy::EventState_Tutorial_EatEnemy(Scene_Base* pScene) : EventState_Tutorial_Base(pScene)
{
    // 敵に注目する
    pCamera_->SetTarget(pGameScene_->Get_pStageManager()->GetCurrentStage()->GetEnemyManager()->GetEnemy(0));
    pCamera_->SetInterpolationRate(0.05f);

    // テキストのステップ数などを設定
    textStepMax_ = 5;
    spriteClipHeightOffset_ = 63.0f * 3;
    textSprite_->clipLT.y = spriteClipHeightOffset_;
}

EventState_Tutorial_EatEnemy::~EventState_Tutorial_EatEnemy(){
    pPlayer_->SetIsMovable(true);
    pCamera_->SetTarget(pPlayer_);
    pCamera_->SetInterpolationRate(0.075f);
}
