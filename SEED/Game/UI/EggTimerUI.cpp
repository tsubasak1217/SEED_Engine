#include "EggTimerUI.h"

EggTimerUI::EggTimerUI(){}

EggTimerUI::~EggTimerUI(){}

void EggTimerUI::Initialize(){
    //=================== UIの初期化 ===================//
    backEggUI_ = std::make_unique<Sprite>("selectScene/egg.png");

    backEggUI_->anchorPoint = {0.5f,0.5f};
    backEggUI_->translate = {1180.f,620.f};
    backEggUI_->size = {128.f,128.f};
    backEggUI_->color = {.6f,.6f,.6f,1.f};

    leftTimeUI_ = std::make_unique<Sprite>("GameUI/numbers.png");

    leftTimeUI_->scale = {2.f,2.f};
    leftTimeUI_->anchorPoint = {0.5f,0.5f};
    leftTimeUI_->translate = {1180.f,620.f};

    leftTimeUI_->clipSize = {32.f,32.f};
    leftTimeUI_->clipLT = {0.f,0.f};
}

void EggTimerUI::Update(float _leftTime){
    //=================== 残り時間に対応した数字を表示 ===================//
    leftTimeUI_->clipLT.x = float(int(leftTimeUI_->clipSize.x) * int(_leftTime));
}

void EggTimerUI::Draw(){
    backEggUI_->Draw();
    leftTimeUI_->Draw();
}
