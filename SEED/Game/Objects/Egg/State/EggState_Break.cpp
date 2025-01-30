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

EggState_Break::EggState_Break(BaseCharacter* character,bool breakToNextStage){
    Initialize("Break",character);
    breakToNextStage_ = breakToNextStage;
}

EggState_Break::~EggState_Break(){}

void EggState_Break::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    JsonCoordinator::RegisterItem("Egg","BreakTime",breakTime_);

    leftTime_ = breakTime_;

    //==================== UI ====================//
    timerUi_ = std::make_unique<EggTimerUI>();
    timerUi_->Initialize();

}

void EggState_Break::Update(){
    leftTime_ -= ClockManager::DeltaTime();
    // ボタン押したら 即孵化
    if(PlayerInput::CharacterMove::FastForwardEggTimer()){
        leftTime_ = 0.f;
    }

    timerUi_->Update(leftTime_);

    ManageState();
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

                    { //! 次のステージへの遷移失敗することがあるので 仮対処
                        pPlayer->SetPosition(StageManager::GetStartPos());
                    }

                    { //前ステージの死体を消す
                        PlayerCorpseManager* pCorpseManager = pPlayer->GetCorpseManager();
                        pCorpseManager->RemoveAll();
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