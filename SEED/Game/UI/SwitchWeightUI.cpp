#include "SwitchWeightUI.h"


#include "CameraManager.h"
#include "FieldObject/Switch/FieldObject_Switch.h"

SwitchWeightUI::SwitchWeightUI(FieldObject_Switch* _host):host_(_host){}

SwitchWeightUI::~SwitchWeightUI(){}

const float whightUIOffsetY = 6.f;
void SwitchWeightUI::Initialize(){
    //=================== UIの初期化 ===================//
    //------------------- 1桁目 -----------------------//
    weightUI_[0] = std::make_unique<Sprite>("GameUI/num.png");
    weightUI_[0]->scale = {0.75f,0.75f};
    weightUI_[0]->translate = CameraManager::GetActiveCamera()->ToScreenPosition(host_->GetWorldTranslate());
    weightUI_[0]->translate.y += whightUIOffsetY;
    weightUI_[0]->color = MyMath::FloatColor(0x173b4cff);
    weightUI_[0]->clipSize = {39.f,56.f};
    weightUI_[0]->clipLT = {0.f,0.f};

    // 10以上なら 2桁目
    if(host_->GetWeight() >= 10){
        isDoubleDigit_ = true;
        weightUI_[1] = std::make_unique<Sprite>("GameUI/num.png");
        weightUI_[1]->scale = {0.9f,0.9f};
        weightUI_[1]->translate = CameraManager::GetActiveCamera()->ToScreenPosition(host_->GetWorldTranslate() + Vector3(0.0f, whightUIOffsetY,0.0f));
        weightUI_[1]->color = MyMath::FloatColor(0x173b4cff);
        weightUI_[1]->clipSize = {39.f,56.f};
        weightUI_[1]->clipLT = {0.f,0.f};

        weightUI_[0]->anchorPoint = {0.f,0.5f};
        weightUI_[1]->anchorPoint = {1.f,0.5f};
    } else{
        weightUI_[0]->anchorPoint = {0.5f,0.5f};
    }

    //===================== Back 
    back_ = std::make_unique<Sprite>("GameUI/numFrame.png");
    back_->anchorPoint = {0.5f,0.5f};
    back_->size = {62.f,62.f};
}

void SwitchWeightUI::Update(){
    int hostWheight = host_->GetRequiredWeight();

    // 座標更新
    const Vector3& hostPos = host_->GetWorldTranslate();
    weightUI_[0]->translate = CameraManager::GetActiveCamera()->ToScreenPosition(hostPos);
    weightUI_[0]->translate.y += whightUIOffsetY;
    if(hostWheight >= 10){
        isDoubleDigit_ = true;
        //座標更新
        weightUI_[1]->translate =  weightUI_[0]->translate;

        int wheightInt[2];
        wheightInt[1] =  int(hostWheight) / 10;
        wheightInt[0] = (int(hostWheight) - wheightInt[1]) / 10;

        for(size_t i = 0; i < 2; i++){
            weightUI_[i]->clipLT.x = float(int(weightUI_[i]->clipSize.x) * wheightInt[i]);
        }
    } else{
        weightUI_[0]->clipLT.x = float(int(weightUI_[0]->clipSize.x) * int(hostWheight));
    }

    back_->translate = weightUI_[0]->translate;
}

void SwitchWeightUI::Draw(){
    back_->Draw();
    weightUI_[0]->Draw();
    if(isDoubleDigit_){
        weightUI_[1]->Draw();
    }
}