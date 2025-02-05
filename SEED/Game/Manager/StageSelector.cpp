#include "StageSelector.h"
#include "InputManager/InputManager.h"
#include "ClockManager.h"
#include "Environment.h"
#include "FieldObject/ViewPoint/FieldObject_ViewPoint.h"

//lib
#include "../PlayerInput/PlayerInput.h"

///////////////////////////////////////////////////////////////////////////
// コンストラクタ
///////////////////////////////////////////////////////////////////////////

StageSelector::StageSelector(StageManager* stageManager,FollowCamera* camera){
    pStageManager_ = stageManager;
    pCamera_ = camera;
    pCamera_->SetInterpolationRate(0.075f);
    Initialize();
}

StageSelector::~StageSelector(){
    pCamera_->SetDistance(50.0f);
}

///////////////////////////////////////////////////////////////////////////
// 初期化処理
///////////////////////////////////////////////////////////////////////////
void StageSelector::Initialize(){
    SEED::SetCamera("follow");

    // フレーム
    frame_ = std::make_unique<Sprite>("SelectScene/frame.png");

    // ステージ名画像
    stageName_ = std::make_unique<Sprite>("SelectScene/stageNames.png");
    stageName_->anchorPoint = {0.5f,0.5f};
    stageName_->translate = {kWindowCenter.x,70.0f};
    stageName_->clipSize = {545.0f,140.0f};

    // 左右の矢印
    for(int i = 0; i < 2; i++){
        arrow_[i] = std::make_unique<Sprite>("SelectScene/arrow.png");
        arrow_[i]->anchorPoint = {0.5f,0.5f};
        arrow_[i]->translate = {190.0f + i * 900.0f,kWindowCenter.y};
        arrow_[i]->flipX = i;
    }

    // 集める星
    for(int i = 0; i < 3; i++){
        collectionStars_[i] = std::make_unique<Sprite>("SelectScene/star.png");
        collectionStars_[i]->translate = Vector2(85.0f,65.0f) + Vector2(90.0f * i,0.0f);
    }

    // 難易度表示の卵
    for(int i = 0; i < 5; i++){
        difficultyEggs_[i] = std::make_unique<Sprite>("SelectScene/egg.png");
        difficultyEggs_[i]->translate = Vector2(923.0f,612.0f) + Vector2(60.0f * i,0.0f);
        difficultyEggs_[i]->layer = 5 - i;
    }

    // ステージの状態に合わせて更新
    UpdateItems(0);

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
    SpriteMotion();
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

    int step = int(PlayerInput::StageSelect::addStageNum()) - int(PlayerInput::StageSelect::subStageNum());

    // スティックの入力からステージを選択
    if(step != 0){
        pStageManager_->StepStage(step);
        UpdateItems(step);
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
    if(PlayerInput::StageSelect::DecideStage()){
        isDecided_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////
// アイテムの情報更新
///////////////////////////////////////////////////////////////////////////
void StageSelector::UpdateItems(int32_t step){

    /*-------------------------------------*/
    // ステージ名の更新
    /*-------------------------------------*/
    stageName_->clipLT = {0.0f,140.0f * StageManager::GetCurrentStageNo()};

    /*-------------------------------------*/
    // 集める星の更新
    /*-------------------------------------*/

    // 現在のステージの集めた星の数分、明るく表示
    for(int32_t i = 0; i < 3; i++){
        collectionStars_[i]->color = {0.2f,0.2f,0.2f,0.9f};
        if(i < StageManager::GetCurrentStageStarCount()){
            collectionStars_[i]->color = {1.0f,1.0f,1.0f,1.0f};
        }
    }

    /*-------------------------------------*/
    // 難易度表示の卵の更新
    /*-------------------------------------*/

    // 現在のステージの難易度分、明るく表示
    for(int32_t i = 0; i < 5; i++){
        difficultyEggs_[i]->color = {0.2f,0.2f,0.2f,0.9f};
        if(i < (int)StageManager::GetCurrentStage()->GetDifficulty()){
            i = std::clamp(i, 0, 4);
            difficultyEggs_[i]->color = {1.0f,1.0f,1.0f,1.0f};
        }
    }

    /*-------------------------------------*/
    // 矢印の表示情報更新
    /*-------------------------------------*/

    // 最初のステージの場合左を非表示
    if(StageManager::GetCurrentStageNo() == 0){
        arrow_[0]->color = {0.0f,0.0f,0.0f,0.0f};
    } else{
        arrow_[0]->color = {1.0f,1.0f,1.0f,1.0f};
    }

    // 最後のステージの場合右を非表示
    if(StageManager::GetCurrentStageNo() == StageManager::GetStageCount() - 1){
        arrow_[1]->color = {0.0f,0.0f,0.0f,0.0f};
    } else{
        arrow_[1]->color = {1.0f,1.0f,1.0f,1.0f};
    }

    // 進んだ方向の矢印を一時的に大きくする
    if(step != 0){
        // 矢印のスケールを変更
        step < 0 ? arrow_[0]->scale = {1.3f,1.3f} : arrow_[1]->scale = {1.3f,1.3f};
    }


    /*-------------------------------------*/
    // カメラの情報をセット
    /*-------------------------------------*/

    // ステージから注目点を取得
    BaseObject* pTarget = pStageManager_->GetCurrentStage()->GetViewPoint();

    // 注目点をカメラにセット
    if(pTarget){
        pCamera_->SetTarget(pTarget);
        if(FieldObject_ViewPoint* pViewPoint = dynamic_cast<FieldObject_ViewPoint*>(pTarget)){
            pCamera_->SetDistance(pViewPoint->distance_);
        }
    }

    // カメラの初期位置をセット
    pCamera_->SetPhi(3.14f * 0.3f);
}


///////////////////////////////////////////////////////////////////////////
// スプライトの動き
///////////////////////////////////////////////////////////////////////////
void StageSelector::SpriteMotion(){

    /*----------------------------------------*/
    // 矢印の動き
    /*----------------------------------------*/

    // 矢印を左右に動かすためのsin用の角度
    static float arrowTheta = 0.0f;
    arrowTheta += (6.28f * 0.5f) * ClockManager::DeltaTime();// 2秒で一周
    // 実際に動かす
    static Vector2 basePos[2] = {arrow_[0]->translate,arrow_[1]->translate};
    arrow_[0]->translate.x = basePos[0].x + 10.0f * sin(arrowTheta);
    arrow_[1]->translate.x = basePos[1].x + -10.0f * sin(arrowTheta);
    // スケールが1になるようにする
    for(int i = 0; i < 2; i++){
        arrow_[i]->scale.x += (1.0f - arrow_[i]->scale.x) * 0.1f * ClockManager::TimeRate();
        arrow_[i]->scale.y += (1.0f - arrow_[i]->scale.y) * 0.1f * ClockManager::TimeRate();
    }
}
