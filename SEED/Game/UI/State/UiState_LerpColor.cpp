#include "UiState_LerpColor.h"

//host
#include "../UI.h"

//lib
#include "ClockManager.h"
//externals
#include "../adapter/json/JsonCoordinator.h"

//math
#include "MyMath.h"

UiState_LerpColor::UiState_LerpColor(UI* _ui,const std::string& _stateName)
    :IUiState(_ui,_stateName){}
UiState_LerpColor::UiState_LerpColor(UI* _ui,const std::string& _stateName,float _elapsedTime)
    :IUiState(_ui,_stateName),elapsedTime_(_elapsedTime){}

UiState_LerpColor::~UiState_LerpColor(){}

void UiState_LerpColor::Initialize(){
    startColor_ = ui_->GetColor();

    //================= Jsonから読み込み =================//
    JsonCoordinator::RegisterItem(ui_->GetName(),stateName_ + "EndColor",endColor_);
    JsonCoordinator::RegisterItem(ui_->GetName(),stateName_ + "LerpDulation",duration_);
}

void UiState_LerpColor::Update(){
    elapsedTime_ += ClockManager::DeltaTime();
    if(elapsedTime_ > duration_){
        elapsedTime_ = duration_;
    }

    ui_->SetColor(MyMath::Lerp(startColor_,endColor_,elapsedTime_ / duration_));
}

void UiState_LerpColor::Draw(){}

void UiState_LerpColor::BeginFrame(){}

void UiState_LerpColor::EndFrame(){}
