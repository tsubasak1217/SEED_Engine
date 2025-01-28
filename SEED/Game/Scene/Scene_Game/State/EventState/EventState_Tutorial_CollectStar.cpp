#include "EventState_Tutorial_CollectStar.h"

//lib
#include "../PlayerInput/PlayerInput.h"

EventState_Tutorial_CollectStar::EventState_Tutorial_CollectStar(){}

EventState_Tutorial_CollectStar::EventState_Tutorial_CollectStar(Scene_Base* pScene)
    : EventState_Base(pScene){
    pGameScene_ = dynamic_cast<Scene_Game*>(pScene);
    pCamera_ = pGameScene_->Get_pCamera();
    pPlayer_ = pGameScene_->Get_pPlayer();

    // テキストの初期化
    textFieldSprite_ = std::make_unique<Sprite>("Tutorials/textField.png");
    textFieldSprite_->color = {1.0f,1.0f,1.0f,0.0f};

    textSprite_ = std::make_unique<Sprite>("Tutorials/tutorialText.png");
    textSprite_->anchorPoint = {0.5f,0.5f};
    textSprite_->clipSize = {1000.0f,63.0f};
    textSprite_->clipLT.y = spriteClipHeightOffset_;
    textSprite_->translate = {640.0f,600.0f};
    textSprite_->color = {1.0f,1.0f,1.0f,0.0f};
}

void EventState_Tutorial_CollectStar::Initialize(){}

void EventState_Tutorial_CollectStar::Update(){
    static float alpha = 0.0f;
    static float alpha2 = 0.0f;

    // テキストを進める処理
    if(textStep_ < textStepMax_){
        time_ += ClockManager::DeltaTime();
        alpha = std::clamp((time_ - 1.0f) / 1.0f,0.0f,1.0f);

        if(textStep_ == 0){
            alpha2 = std::clamp((time_ - 1.0f) / 1.0f,0.0f,1.0f);
        }

        if(PlayerInput::Tutorial::StepText()){
            if(time_ > 2.0f){

                textStep_++;
                time_ = 1.0f;

            } else{
                time_ = 2.0f;
            }
        }

    } else{

        time_ -= 0.5f * ClockManager::DeltaTime();
        alpha = std::clamp(time_ / 2.0f,0.0f,1.0f);
        alpha2 = std::clamp(time_ / 2.0f,0.0f,1.0f);

    }

    // テキストの透明度の更新
    textSprite_->color.w = alpha;
    textFieldSprite_->color.w = alpha2;

    // テキストの切り抜き位置の更新
    textSprite_->clipLT.y = spriteClipHeightOffset_ + 63.0f * std::clamp(textStep_,0,textStepMax_ - 1);


    // テキストの進行が終了したら
    if(textStep_ >= textStepMax_){
        if(time_ < 0.0f){
            pGameScene_->EndEvent();
        }
    }
}

void EventState_Tutorial_CollectStar::Draw(){
    textFieldSprite_->Draw();
    textSprite_->Draw();
}
