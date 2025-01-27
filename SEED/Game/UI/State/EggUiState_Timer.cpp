#include "EggUiState_Timer.h"

//host
#include "../UI.h"

///stl
#include <algorithm>

///local
//object
#include "Egg/Egg.h"
//lib
#include "CameraManager.h"
#include "ClockManager.h"
//externals
#include "../adapter/json/JsonCoordinator.h"

EggUiState_Timer::EggUiState_Timer(UI* _ui,
                                   Egg* _egg,
                                   const std::string& _stateName,
                                   float _duration,
                                   float* _leftTime):
    IUiState(_ui,_stateName),
    egg_(_egg),
    duration_(_duration),
    leftTime_(_leftTime){}

EggUiState_Timer::~EggUiState_Timer(){}

const Vector2 numberUiSize_ = {32.f,32.f};
void EggUiState_Timer::Initialize(){
    //=================== UIの初期化 ===================//
    leftTimeNumber_ = std::make_unique<UI>("EggLeftTimeNumberUI");
    leftTimeNumber_->Initialize("GameUI/numbers.png");
    leftTimeNumber_->SetClipSize(numberUiSize_);

    //=================== Json ===================//
    JsonCoordinator::RegisterItem("EggLeftTimeNumberUI","Offset",numberUiOffset_);
}

void EggUiState_Timer::Update(){
    //=================== 座標の計算,更新 ===================//
    uiPos_ = CameraManager::GetActiveCamera()->ToScreenPosition(egg_->GetWorldTranslate());

    uiPos_.x = std::clamp(uiPos_.x,min_.x,max_.x);
    uiPos_.y = std::clamp(uiPos_.y,min_.y,max_.y);

    ui_->SetTranslate(uiPos_);
    leftTimeNumber_->SetTranslate(uiPos_ + numberUiOffset_);

    //=================== 残り時間に対応した数字を表示 ===================//
    leftTimeNumber_->SetClipLTX(numberUiSize_.x * int(*leftTime_));

    leftTimeNumber_->BeginFrame();
    leftTimeNumber_->Update();
    leftTimeNumber_->EndFrame();
}

void EggUiState_Timer::Draw(){
    leftTimeNumber_->Draw();
}

void EggUiState_Timer::BeginFrame(){}

void EggUiState_Timer::EndFrame(){}
