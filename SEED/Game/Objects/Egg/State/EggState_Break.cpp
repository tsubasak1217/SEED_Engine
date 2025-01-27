#include "EggState_Break.h"

//object
#include "../Egg.h"
#include "StageManager.h"

//uiState
#include "../UI/State/EggUiState_Timer.h"

//camera
#include "FollowCamera.h"
//manager
#include "ClockManager.h"
#include "Egg/Manager/EggManager.h"
//lib
#include "../adapter/json/JsonCoordinator.h"

EggState_Break::EggState_Break(BaseCharacter* character,bool breakToNextStage){
    Initialize("Break",character);
    breakToNextStage_ = breakToNextStage;
}

EggState_Break::~EggState_Break(){
    timerUi_->Finalize();
}

void EggState_Break::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    JsonCoordinator::RegisterItem("Egg","BreakTime",breakTime_);
    JsonCoordinator::RegisterItem("Egg","BreakTimeFactor",timeFactor_);

    leftTime_ = breakTime_;

    //==================== UI ====================//
    timerUi_ = std::make_unique<UI>("EggTimerUI");
    timerUi_->Initialize("SelectScene/egg.png");

    Egg* egg = dynamic_cast<Egg*>(pCharacter_);
    auto eggUiState = std::make_unique<EggUiState_Timer>(
        timerUi_.get(),
        egg,
        "BreakTimeTimer",
        breakTime_,
        &leftTime_
    );
    eggUiState->Initialize();
    timerUi_->SetState(std::move(eggUiState));
}

void EggState_Break::Update(){
    timerUi_->BeginFrame();

    if(Input::GetInstance()->IsPressPadButton(PAD_BUTTON::X)){
        leftTime_ -= ClockManager::DeltaTime() * timeFactor_;
    } else{
        leftTime_ -= ClockManager::DeltaTime();
    }

    timerUi_->Update();

    ManageState();

    timerUi_->EndFrame();
}

void EggState_Break::Draw(){
    timerUi_->Draw();
}

void EggState_Break::ManageState(){
    if(leftTime_ <= 0.0f){
        Egg* egg = dynamic_cast<Egg*>(pCharacter_);
        if(egg){
            egg->Break();

            if(breakToNextStage_){
                // 次のステージに進める
                StageManager::StepStage(1);
                StageManager::SetIsHandOverColliderNext(false);
                breakToNextStage_ = false;

                { // Playerを必要とする処理
                    Player* pPlayer = egg->GetEggManager()->GetPlayer();
                    {   // プレイヤーに次のステージの敵情報を渡す
                        EnemyManager* enemyManager = StageManager::GetCurrentStage()->GetEnemyManager();
                        pPlayer->SetEnemyManager(enemyManager);
                    }

                    {
                        // カメラのターゲットをPlayerに変更
                        FollowCamera* pFollowCamera =  dynamic_cast<FollowCamera*>(pPlayer->GetFollowCamera());
                        pFollowCamera->SetTarget(pPlayer);
                    }
                }

            }
        }
    }
}