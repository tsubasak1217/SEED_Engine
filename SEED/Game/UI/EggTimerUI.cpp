#include "EggTimerUI.h"

///math
#include "Easing.h"

EggTimerUI::EggTimerUI(){}

EggTimerUI::~EggTimerUI(){}

void EggTimerUI::Initialize(){
    //=================== UIの初期化 ===================//
    for(auto& timeUI : leftTimeUI_){
        timeUI = std::make_unique<Sprite>("GameUI/numbers.png");
        timeUI->scale = {2.f,2.f};
        timeUI->translate = {1155.f,559.f};
        timeUI->color = {0.6f,0.6f,0.6f,1.f};
        timeUI->clipSize = {32.f,32.f};
        timeUI->clipLT = {0.f,0.f};
    }
    // 1桁目 .. [0]
    // 2桁目 .. [1]
    leftTimeUI_[0]->anchorPoint = {0.f,0.5f};
    leftTimeUI_[1]->anchorPoint = {1.f,0.5f};
}

void EggTimerUI::Update(float _leftTime){
    if(isDoubleDigit_){
        isDoubleDigit_ = _leftTime >= 10.f;
    }

    if(isDoubleDigit_){
        //=================== 残り時間に対応した数字を表示(2桁) ===================//
        int leftTimeInt[2];
        leftTimeInt[1] =  int(_leftTime) / 10;
        leftTimeInt[0] = (int(_leftTime) - leftTimeInt[1]) / 10;

        for(size_t i = 0; i < 2; i++){
            leftTimeUI_[i]->clipLT.x = float(int(leftTimeUI_[i]->clipSize.x) * leftTimeInt[i]);
        }
    } else{
        //=================== 残り時間に対応した数字を表示 ===================//
        leftTimeUI_[0]->anchorPoint = {0.5f,0.5f};
        leftTimeUI_[0]->clipLT.x = float(int(leftTimeUI_[0]->clipSize.x) * int(_leftTime));
    }
}

void EggTimerUI::Draw(){
    leftTimeUI_[0]->Draw();
    if(isDoubleDigit_){
        leftTimeUI_[1]->Draw();
    }
}
