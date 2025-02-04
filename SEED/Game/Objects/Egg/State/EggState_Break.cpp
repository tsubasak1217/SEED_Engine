#include "EggState_Break.h"

//object
#include "../Egg.h"
#include "StageManager.h"

//camera
#include "FollowCamera.h"
//manager
#include "ClockManager.h"
#include "Egg/Manager/EggManager.h"
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"
//lib
#include "../adapter/json/JsonCoordinator.h"
#include "../PlayerInput/PlayerInput.h"
//math
#include "Easing.h"

EggState_Break::EggState_Break(BaseCharacter* character){
    JsonCoordinator::RegisterItem("Egg","BreakTime",breakTime_);

    Initialize("Break",character);
}

EggState_Break::EggState_Break(BaseCharacter* character,float _breakTime){
    breakTime_ = _breakTime;

    Initialize("Break",character);
}

EggState_Break::~EggState_Break(){}

void EggState_Break::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);
    leftTime_ = breakTime_;

    //==================== UI ====================//
    timerUi_ = std::make_unique<EggTimerUI>();
    timerUi_->Initialize();

    timeSkipButtonUI_ = std::make_unique<Sprite>("GameUI/A.png");
    timeSkipButtonUI_->anchorPoint = Vector2(0.5f,0.5f);
    timeSkipButtonUI_->translate = timerUi_->GetUIPos();
    timeSkipButtonUI_->translate.x -= timerUi_->GetUISize().x * 0.5f + timeSkipButtonUI_->size.x * 0.5f;
}

void EggState_Break::Update(){
    leftTime_ -= ClockManager::DeltaTime();
    // ボタン押したら 即孵化
    if(PlayerInput::CharacterMove::FastForwardEggTimer()){
        leftTime_ = 0.f;
    }

    timerUi_->Update(leftTime_);
    ButtonUISinAnimation();

    ManageState();
}

void EggState_Break::Draw(){
    timerUi_->Draw();
    timeSkipButtonUI_->Draw();
}

void EggState_Break::ManageState(){
    if(leftTime_ <= 0.0f){
        Egg* egg = dynamic_cast<Egg*>(pCharacter_);
        if(egg){
            egg->SpawnPlayer();
        }
    }
}

void EggState_Break::ButtonUISinAnimation(){

    timeSkipButtonUI_->color.w = EaseInQuart(cosf(leftTime_));
}
