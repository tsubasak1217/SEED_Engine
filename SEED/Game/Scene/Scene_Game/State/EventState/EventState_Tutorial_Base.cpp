#include "EventState_Tutorial_Base.h"
#include "Player/PlayerState/PlayerState_Idle.h"

//static変数の初期化
std::unique_ptr<Sprite> EventState_Tutorial_Base::textFieldSprite_;
std::unique_ptr<Sprite> EventState_Tutorial_Base::textSprite_;
std::unique_ptr<Sprite> EventState_Tutorial_Base::skipButtonUI_;
std::unique_ptr<Sprite> EventState_Tutorial_Base::progressButtonUI_;

EventState_Tutorial_Base::EventState_Tutorial_Base(){
}

/////////////////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////
EventState_Tutorial_Base::EventState_Tutorial_Base(Scene_Base* pScene){
    Initialize(pScene);
}

EventState_Tutorial_Base::~EventState_Tutorial_Base(){
}


/////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
/////////////////////////////////////////////////////////////////////////////////////

void EventState_Tutorial_Base::Initialize(Scene_Base* pScene){
    pGameScene_ = dynamic_cast<Scene_Game*>(pScene);
    pCamera_ = pGameScene_->Get_pCamera();
    pPlayer_ = pGameScene_->Get_pPlayer();
    pPlayer_->SetIsMovable(false);
    pPlayer_->ChangeState(new PlayerState_Idle("Player_Idle", pPlayer_));

    // テキストの初期化
    textFieldSprite_ = std::make_unique<Sprite>("Tutorials/textField.png");
    textFieldSprite_->color = { 1.0f,1.0f,1.0f,0.0f };
    textFieldSprite_->isStaticDraw = false;

    textSprite_ = std::make_unique<Sprite>("Tutorials/tutorialText.png");
    textSprite_->anchorPoint = { 0.5f,0.5f };
    textSprite_->clipSize = { 1000.0f,63.0f };
    textSprite_->translate = { 640.0f,600.0f };
    textSprite_->color = { 1.0f,1.0f,1.0f,0.0f };
    textSprite_->isStaticDraw = false;

    // skipButtonUI
    skipButtonUI_ = std::make_unique<Sprite>("GameUI/skip.png");
    skipButtonUI_->anchorPoint = {0.5f,0.5f};
    skipButtonUI_->translate = {1114.f,50.f};

    // progressButtonUI
    progressButtonUI_ = std::make_unique<Sprite>("GameUI/a.png");
    progressButtonUI_->anchorPoint = {0.5f,0.5f};
    progressButtonUI_->translate = {1114.f,680.f};

    //
    kTime_ = 1.0f;
}


/////////////////////////////////////////////////////////////////////////////////////
// テキストの進行処理
/////////////////////////////////////////////////////////////////////////////////////
void EventState_Tutorial_Base::Update(){
    EventState_Base::Update();

    static float alpha = 0.0f;// テキストの透明度
    static float alpha2 = 0.0f;// テキストの背景の透明度

    // テキストを進める処理
    if(textStep_ < textStepMax_){
        time_ += ClockManager::DeltaTime();

        if(PlayerInput::Tutorial::StepText()){
            if(time_ > 2.0f){

                textStep_++;
                time_ = kTime_;

            } else{
                time_ = 2.0f;
            }
        }

        if(PlayerInput::Tutorial::SkipTutorial()){
            textStep_ = textStepMax_;
            time_ = 0.25f;
        }

        // テキストの透明度を徐々に濃くしていく(2秒かけて最大へ。1秒目から出現)
        alpha = std::clamp((time_ - kTime_) / kTime_, 0.0f, 1.0f);
        if(textStep_ == 0){
            alpha2 = std::clamp((time_ - kTime_) / kTime_, 0.0f, 1.0f);
        }

    } else{// テキストが進行し終わったら薄くしていく

        time_ -= 0.5f * ClockManager::DeltaTime();
        alpha = std::clamp(time_ / (kTime_ * 2.0f), 0.0f, 1.0f);
        alpha2 = std::clamp(time_ / (kTime_ * 2.0f), 0.0f, 1.0f);

    }

    // テキストの透明度の更新
    textSprite_->color.w = alpha;
    textFieldSprite_->color.w = alpha2;

    // テキストの切り抜き位置の更新
    textSprite_->clipLT.y = spriteClipHeightOffset_ + 63.0f * std::clamp(textStep_, 0, textStepMax_ - 1);


    // テキストの進行が終了したら
    if(textStep_ >= textStepMax_){
        if(time_ < 0.0f){
            // カメラのターゲットと補間速度をプレイヤーに戻し終了
            pCamera_->SetTarget(pPlayer_);
            pCamera_->SetInterpolationRate(0.15f);
            pGameScene_->EndEvent();
        }
    }

}

/////////////////////////////////////////////////////////////////////////////////////
// 描画処理
/////////////////////////////////////////////////////////////////////////////////////
void EventState_Tutorial_Base::Draw(){
    textFieldSprite_->Draw();
    textSprite_->Draw();
    skipButtonUI_->Draw();
    progressButtonUI_->Draw();
}
