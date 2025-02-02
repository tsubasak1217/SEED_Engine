#pragma once
#include <array>
#include <memory>
#include "../../SEED/Lib/patterns/ISubject.h"
#include "../Manager/Stage.h"

class Player;

class StageManager{
public:
    StageManager();
    StageManager(ISubject& subject);
    ~StageManager();
    StageManager& operator =(StageManager&) = default;

public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();

    /// <summary>
    /// 星の数を更新(現在のステージで取得した星の数が今までで一番多いければ,記録する)
    /// </summary>
    void UpdateStarCont(uint32_t stage);
    void UpdateStarContOnCurrentStage(){
        UpdateStarCont(StageManager::GetCurrentStageNo());
    }
public:
    void HandOverColliders();
    static void StepStage(int32_t step);

private:
    void LoadStages();

private:
    static const int32_t kStageCount_ = 10;
    static int32_t currentStageNo_;
    static int32_t preStageNo_;
    static std::array<std::unique_ptr<Stage>, kStageCount_> stages_;
    static std::array<int, kStageCount_> getStarCounts_;
    static std::array<bool,kStageCount_> clearStatus_; // ステージのクリア状態
    static bool isPlaying_;
    static bool isHandOverColliderNext_;

    // playerのポインタ
    Player* pPlayer_;

public:
    void SetPlayer(Player* pPlayer);
    Player* GetPlayerPtr(){ return pPlayer_; }
    static int32_t GetStageCount(){ return kStageCount_; }
    std::array<std::unique_ptr<Stage>, kStageCount_>& GetStages(){ return stages_; }
    static Stage* GetCurrentStage(){ return stages_[currentStageNo_].get(); }
    Stage* GetPreStage(){ return stages_[preStageNo_].get(); }
    static int32_t GetCurrentStageNo(){ return currentStageNo_; }
    static int GetCurrentStageStarCount(){ return getStarCounts_[currentStageNo_]; }
    static void GetCurrentStageStarCount(int starCount){ getStarCounts_[currentStageNo_] = starCount; }
    static bool GetIsClear(int32_t stageNo){ return clearStatus_[stageNo]; }
    static void SetIsClear(int32_t stageNo,bool isClear){ clearStatus_[stageNo] = isClear; }
    static void StageClear(int32_t stageNo){ clearStatus_[stageNo] = true; }
    static void SetCurrentStageNo(int32_t stageNo){ currentStageNo_ = stageNo; }
    static Vector3 GetStartPos();
    static Vector3 GetNextStartPos();
    static bool IsStageChanged(){ return currentStageNo_ != preStageNo_; }
    static bool IsPlaying(){ return isPlaying_; }
    static bool IsLastStage(){ return currentStageNo_ == kStageCount_ - 1; }
    static bool IsHandOverColliderNext(){ return isHandOverColliderNext_; }
    static void SetIsHandOverColliderNext(bool isHandOverColliderNext){ isHandOverColliderNext_ = isHandOverColliderNext; }
};