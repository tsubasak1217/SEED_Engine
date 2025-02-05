#include "EventState_Tutorial_Corpse.h"
#include "PlayerCorpse/PlayerCorpse.h"

EventState_Tutorial_Corpse::EventState_Tutorial_Corpse(){
}

EventState_Tutorial_Corpse::EventState_Tutorial_Corpse(Scene_Base* pScene) : EventState_Tutorial_Base(pScene)
{
    textStepMax_ = 2;
    spriteClipHeightOffset_ = 63.0f * 25;
    textSprite_->clipLT.y = spriteClipHeightOffset_;

    // 死体に注目する
    pCamera_->SetTarget(StageManager::GetCurrentStage()->GetPlayerCorpseManager()->GetPlayerCorpse(0));
    pCamera_->SetInterpolationRate(0.05f);
}

EventState_Tutorial_Corpse::~EventState_Tutorial_Corpse(){
    // 死体からPlayerに視線を戻す
    pCamera_->SetTarget(pPlayer_);
    pCamera_->SetInterpolationRate(0.075f);
    pPlayer_->SetIsMovable(true);
}
