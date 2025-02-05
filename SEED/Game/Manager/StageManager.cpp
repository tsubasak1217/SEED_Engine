#include "StageManager.h"

// 各ブロックのヘッダーファイル
#include "FieldObject/Door/FieldObject_Door.h"
#include "FieldObject/GrassSoil/FieldObject_GrassSoil.h"
#include "FieldObject/Soil/FieldObject_Soil.h"
#include "FieldObject/Star/FieldObject_Star.h"
#include "FieldObject/Start/FieldObject_Start.h"
#include "FieldObject/Goal/FieldObject_Goal.h"
#include "FieldObject/Switch/FieldObject_Switch.h"
#include "FieldObject/ViewPoint/FieldObject_ViewPoint.h"

//lib
#include "../adapter/json/JsonCoordinator.h"

///////////////////////////////////////////////////////////////////////
// 静的メンバ変数
///////////////////////////////////////////////////////////////////////
int32_t StageManager::currentStageNo_ = 0;
std::array<std::unique_ptr<Stage>, StageManager::kStageCount_> StageManager::stages_;
std::unique_ptr<Stage> StageManager::titleStage_;
std::array<int, StageManager::kStageCount_> StageManager::getStarCounts_;
std::array<bool, StageManager::kStageCount_> StageManager::clearStatus_;
int32_t StageManager::preStageNo_ = 0;
bool StageManager::isPlaying_ = false;
bool StageManager::isHandOverColliderNext_ = false;
std::array<int, StageManager::kStageCount_> StageManager::difficulties_ = {1,2,3,3,4,4};
bool StageManager::isTitle_ = false;

///////////////////////////////////////////////////////////////////////
// コンストラクタ
///////////////////////////////////////////////////////////////////////
StageManager::StageManager(){}

StageManager::StageManager(ISubject& subject){
    for (int i = 0; i < kStageCount_; i++){
        stages_[i] = std::make_unique<Stage>(subject, i);
    }
    titleStage_ = std::make_unique<Stage>(subject);
}

///////////////////////////////////////////////////////////////////////
// デストラクタ
///////////////////////////////////////////////////////////////////////
StageManager::~StageManager(){}

///////////////////////////////////////////////////////////////////////
// 初期化
///////////////////////////////////////////////////////////////////////
void StageManager::Initialize(){
    LoadStages();
    LoadTitleStage();

    AudioManager::PlayAudio("SE/star_shine.wav", true, 0.f);

    JsonCoordinator::LoadGroup("UserProgress");
    std::string stageLabel;
    for (int i = 0; i < getStarCounts_.size(); ++i){
        stageLabel = "Stage" + std::to_string(i);
        JsonCoordinator::RegisterItem("UserProgress", stageLabel + "_ClearState", clearStatus_[i]);
        JsonCoordinator::RegisterItem("UserProgress", stageLabel + "_StarCount", getStarCounts_[i]);
    }
}

///////////////////////////////////////////////////////////////////////
// 終了処理
///////////////////////////////////////////////////////////////////////
void StageManager::Finalize(){
    //星の獲得状況を保存
    JsonCoordinator::SaveGroup("UserProgress");
}

///////////////////////////////////////////////////////////////////////
// 更新
///////////////////////////////////////////////////////////////////////
void StageManager::Update(){
    for (auto& stage : stages_){
        stage->Update();
    }
}

///////////////////////////////////////////////////////////////////////
// 描画
///////////////////////////////////////////////////////////////////////
void StageManager::Draw(){
    for (auto& stage : stages_){
        stage->Draw();
    }
}

void StageManager::DrawHUD(){
    for (auto& stage : stages_){
        stage->DrawHUD();
    }
}


///////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
///////////////////////////////////////////////////////////////////////
void StageManager::BeginFrame(){

    preStageNo_ = currentStageNo_;

    for (auto& stage : stages_){
        stage->BeginFrame();
    }
}


///////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
///////////////////////////////////////////////////////////////////////
void StageManager::EndFrame(){
    for (auto& stage : stages_){
        stage->EndFrame();
    }

    stages_[currentStageNo_]->StarSEUpdate();
}

void StageManager::UpdateStarCont(uint32_t _stageNo){
    // 現在のステージのみ 星の取得数を更新
    stages_[_stageNo]->UpdateStarCount();
    // 星の最大取得数を 更新
    getStarCounts_[_stageNo] = ( std::max ) (stages_[_stageNo]->GetCurrentStarCount(), getStarCounts_[_stageNo]);
}

///////////////////////////////////////////////////////////////////////
// コライダーを渡す
///////////////////////////////////////////////////////////////////////
void StageManager::HandOverColliders(){
    if (!isTitle_){
        stages_[currentStageNo_]->HandOverColliders();

    }

    // 卵を投げているときはプレイヤーがいない次のステージのコライダーも渡す
    if (isHandOverColliderNext_){
        stages_[std::clamp(currentStageNo_ + 1, 0, kStageCount_ - 1)]->HandOverColliders();
    }
}

///////////////////////////////////////////////////////////////////////
// ステージを変更する
///////////////////////////////////////////////////////////////////////
void StageManager::StepStage(int32_t step){
    currentStageNo_ += step;
    currentStageNo_ = std::clamp(currentStageNo_, 0, kStageCount_ - 1);
}

///////////////////////////////////////////////////////////////////////
// ステージの読み込み
///////////////////////////////////////////////////////////////////////
void StageManager::LoadStages(){
    // ステージの読み込み
    for (int i = 0; i < kStageCount_; i++){
        std::string filepath = "resources/jsons/Stages/stage_" + std::to_string(i + 1) + ".json";
        stages_[i]->LoadFromJson(filepath);

        // 敵の読み込み
        stages_[i]->GetEnemyManager()->LoadEnemies();

        // 難易度の設定
        stages_[i]->SetDifficulty(difficulties_[i]);
    }
}

void StageManager::LoadTitleStage(){
    std::string filepath = "resources/jsons/Stages/stage_title.json";
    titleStage_->LoadFromJson(filepath);
}

void StageManager::SetPlayer(Player* pPlayer){
    pPlayer_ = pPlayer;

    for (int i = 0; i < kStageCount_; i++){
        stages_[i]->SetPlayer(pPlayer);
    }

    titleStage_->SetPlayer(pPlayer);
}

///////////////////////////////////////////////////////////////////////
// スタート地点の取得
///////////////////////////////////////////////////////////////////////
Vector3 StageManager::GetStartPos(){
    return stages_[currentStageNo_]->GetStartPosition();
}

Vector3 StageManager::GetNextStartPos(){
    return stages_[std::clamp(currentStageNo_ + 1, 0, kStageCount_ - 1)]->GetStartPosition();
}

bool StageManager::GetIsClearAllGoal(){
    for (bool clearStatus : clearStatus_){
        if (!clearStatus){
            return false;
        }
    }
    return true;
}

Vector3 StageManager::GetTitleStartPos(){
    return titleStage_->GetStartPosition();
}
