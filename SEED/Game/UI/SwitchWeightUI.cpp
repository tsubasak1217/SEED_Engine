#include "SwitchWeightUI.h"

#include "CameraManager.h"
#include "FieldObject/Switch/FieldObject_Switch.h"

SwitchWeightUI::SwitchWeightUI(FieldObject_Switch* _host):host_(_host){}

SwitchWeightUI::~SwitchWeightUI(){}

const float whightUIOffsetY = 10.f;
void SwitchWeightUI::Initialize(){
    float hostWheight = host_->GetWeight();
    //=================== UIの初期化 ===================//
    weightUI_[0] = std::make_unique<Sprite>("GameUI/numbers.png");
    weightUI_[0]->scale = {2.f,2.f};
    weightUI_[0]->translate = CameraManager::GetActiveCamera()->ToScreenPosition(host_->GetWorldTranslate());
    weightUI_[0]->translate.y += whightUIOffsetY;
    weightUI_[0]->color = {0.6f,0.6f,0.6f,1.f};
    weightUI_[0]->clipSize = {32.f,32.f};
    weightUI_[0]->clipLT = {weightUI_[0]->clipSize.x * (hostWheight - (hostWheight / 10.f)),0.f};
    // 10以上なら 2桁目
    if(host_->GetWeight() >= 10){
        isDoubleDigit_ = true;
        weightUI_[1] = std::make_unique<Sprite>("GameUI/numbers.png");
        weightUI_[1]->scale = {2.f,2.f};
        weightUI_[1]->translate = CameraManager::GetActiveCamera()->ToScreenPosition(host_->GetWorldTranslate());
        weightUI_[1]->translate.y += whightUIOffsetY;
        weightUI_[1]->color = {0.6f,0.6f,0.6f,1.f};
        weightUI_[1]->clipSize = {32.f,32.f};
        weightUI_[1]->clipLT = {weightUI_[0]->clipSize.x * (hostWheight / 10.f),0.f};


        weightUI_[0]->anchorPoint = {0.f,0.5f};
        weightUI_[1]->anchorPoint = {1.f,0.5f};
    } else{
        weightUI_[0]->anchorPoint = {0.5f,0.5f};
    }
}

void SwitchWeightUI::Update(){
    const Vector3& hostPos = host_->GetWorldTranslate();
    weightUI_[0]->translate = CameraManager::GetActiveCamera()->ToScreenPosition(hostPos);
    weightUI_[0]->translate.y += whightUIOffsetY;

    if(isDoubleDigit_){
        weightUI_[1]->translate =  weightUI_[0]->translate;
    }
}

void SwitchWeightUI::Draw(){
    weightUI_[0]->Draw();
    if(isDoubleDigit_){
        weightUI_[1]->Draw();
    }
}