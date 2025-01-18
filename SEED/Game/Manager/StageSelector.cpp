#include "StageSelector.h"
#include "InputManager/InputManager.h"
#include "ClockManager.h"
#include "Environment.h"


///////////////////////////////////////////////////////////////////////////
// コンストラクタ
///////////////////////////////////////////////////////////////////////////

StageSelector::StageSelector(StageManager* stageManager, FollowCamera* camera){
    pStageManager_ = stageManager;
    pCamera_ = camera;
    Initialize();
}

///////////////////////////////////////////////////////////////////////////
// 初期化処理
///////////////////////////////////////////////////////////////////////////
void StageSelector::Initialize(){

    // フレーム
    frame_ = std::make_unique<Sprite>("SelectScene/frame.png");

    // ステージ名画像
    stageName_ = std::make_unique<Sprite>("SelectScene/stageNames.png");
    stageName_->anchorPoint = { 0.5f, 0.5f };
    stageName_->translate = { kWindowCenter.x, 70.0f };
    stageName_->clipSize = { 545.0f,140.0f };

    // 左右の矢印
    for(int i = 0; i < 2; i++){
        arrow_[i] = std::make_unique<Sprite>("SelectScene/arrow.png");
        arrow_[i]->anchorPoint = { 0.5f, 0.5f };
        arrow_[i]->translate = { 190.0f + i * 900.0f,kWindowCenter.y };
        arrow_[i]->flipX = i;
    }

    // 集める星
    for(int i = 0; i < 3; i++){
        collectionStars_[i] = std::make_unique<Sprite>("SelectScene/star.png");
        collectionStars_[i]->translate = Vector2(85.0f, 65.0f) + Vector2(90.0f * i, 0.0f);
    }

    // 難易度表示の卵
    for(int i = 0; i < 5; i++){
        difficultyEggs_[i] = std::make_unique<Sprite>("SelectScene/egg.png");
        difficultyEggs_[i]->translate = Vector2(923.0f, 612.0f) + Vector2(60.0f * i, 0.0f);
        difficultyEggs_[i]->layer = 5 - i;
    }

    // ステージの状態に合わせて更新
    UpdateItems();
}

///////////////////////////////////////////////////////////////////////////
// 終了処理
///////////////////////////////////////////////////////////////////////////
void StageSelector::Finalize(){}

///////////////////////////////////////////////////////////////////////////
// 更新処理
///////////////////////////////////////////////////////////////////////////
void StageSelector::Update(){
    Select();
    CameraUpdate();
    DecideStage();
}

///////////////////////////////////////////////////////////////////////////
// 描画処理
///////////////////////////////////////////////////////////////////////////
void StageSelector::Draw(){

    // フレーム
    frame_->Draw();

    // ステージ名
    stageName_->Draw();

    // 左右の矢印
    for(int i = 0; i < 2; i++){
        arrow_[i]->Draw();
    }

    // 集める星
    for(int i = 0; i < 3; i++){
        collectionStars_[i]->Draw();
    }

    // 難易度表示の卵
    for(int i = 0; i < 5; i++){
        difficultyEggs_[i]->Draw();
    }
}

///////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
///////////////////////////////////////////////////////////////////////////
void StageSelector::BeginFrame(){}

///////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
///////////////////////////////////////////////////////////////////////////
void StageSelector::EndFrame(){}

///////////////////////////////////////////////////////////////////////////
// ステージ選択処理
///////////////////////////////////////////////////////////////////////////
void StageSelector::Select(){

    // スティックの入力からステージを選択
    if(Input::IsTriggerStick(LR::LEFT, DIRECTION::RIGHT)){
        pStageManager_->StepStage(1);
        UpdateItems();
    } else if(Input::IsTriggerStick(LR::LEFT, DIRECTION::LEFT)){
        pStageManager_->StepStage(-1);
        UpdateItems();
    }
}

///////////////////////////////////////////////////////////////////////////
// カメラの更新
///////////////////////////////////////////////////////////////////////////
void StageSelector::CameraUpdate(){

    static float additionTheta = 3.14f / 360.0f;

    if(MyMath::Length(Input::GetStickValue(LR::RIGHT)) == 0.0f){
        // カメラの角度を変更
        pCamera_->AddTheta(additionTheta * ClockManager::TimeRate());
    }
}

///////////////////////////////////////////////////////////////////////////
// ステージの決定
///////////////////////////////////////////////////////////////////////////
void StageSelector::DecideStage(){
    if(Input::IsTriggerPadButton(PAD_BUTTON::A)){
        isDecided_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////
// アイテムの情報更新
///////////////////////////////////////////////////////////////////////////
void StageSelector::UpdateItems(){

    // ステージ名の更新
    stageName_->clipLT = { 0.0f,140.0f * StageManager::GetCurrentStageNo() };

    // 集める星の更新
    for(uint32_t i = 0; i < 3; i++){
        collectionStars_[i]->color = { 0.2f,0.2f,0.2f,0.9f };
        if(i < StageManager::GetCurrentStageStarCount()){
            collectionStars_[i]->color = { 1.0f,1.0f,1.0f,1.0f };
        }
    }

    // 難易度表示の卵の更新
    for(uint32_t i = 0; i < 5; i++){
        difficultyEggs_[i]->color = { 0.2f,0.2f,0.2f,0.9f };
        if(i < StageManager::GetCurrentStage()->GetDifficulty()){
            difficultyEggs_[i]->color = { 1.0f,1.0f,1.0f,1.0f };
        }
    }


    // ステージから注目点を取得
    BaseObject* pTarget = pStageManager_->GetCurrentStage()->GetViewPoint();

    // 注目点をカメラにセット
    if(pTarget){
        pCamera_->SetTarget(pTarget);
    }

    // カメラの初期位置をセット
    pCamera_->SetPhi(3.14f * 0.3f);
    pCamera_->SetDistance(80.0f);
}
