#pragma once
#include <array>
#include <memory>
#include "../Manager/Stage.h"

class BaseCharacter;

class StageManager{
private:
    StageManager();
    StageManager(const StageManager&) = delete;
    StageManager& operator=(const StageManager&) = delete;
    static inline StageManager* instance_{nullptr};

public:
    ~StageManager();
    static StageManager* GetInstance();

public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();

public:
    void HandOverColliders();
    static void StepStage(int32_t step);

private:
    void LoadStages();

private:
    BaseCharacter* pPlayer_ = nullptr;
    static const int32_t kStageCount_ = 6;
    int32_t currentStageNo_;
    int32_t preStageNo_;
    std::array<std::unique_ptr<Stage>, kStageCount_> stages_;
    std::array<bool,kStageCount_> clearStatus_; // ステージのクリア状態
    std::array<int, kStageCount_> difficulties_;
    bool isPlaying_;
    bool isHandOverColliderNext_;


public:
    void SetPlayer(BaseCharacter* pPlayer){ pPlayer_ = pPlayer; }
    BaseCharacter* GetPlayer(){ return pPlayer_; }
    int32_t GetStageCount(){ return kStageCount_; }
    std::array<std::unique_ptr<Stage>, kStageCount_>& GetStages(){ return stages_; }
    Stage* GetCurrentStage(){ return stages_[currentStageNo_].get(); }
    Stage* GetPreStage(){ return stages_[preStageNo_].get(); }
    int32_t GetCurrentStageNo(){ return currentStageNo_; }
    bool GetIsClear(int32_t stageNo){ return clearStatus_[stageNo]; }
    void SetIsClear(int32_t stageNo,bool isClear){ clearStatus_[stageNo] = isClear; }
    void StageClear(int32_t stageNo){ clearStatus_[stageNo] = true; }
    void SetCurrentStageNo(int32_t stageNo){ currentStageNo_ = stageNo; }
    Vector3 GetStartPos();
    Vector3 GetNextStartPos();
    bool IsStageChanged(){ return currentStageNo_ != preStageNo_; }
    bool IsPlaying(){ return isPlaying_; }
    bool IsLastStage(){ return currentStageNo_ == kStageCount_ - 1; }
    bool IsHandOverColliderNext(){ return isHandOverColliderNext_; }
    void SetIsHandOverColliderNext(bool isHandOverColliderNext){ isHandOverColliderNext_ = isHandOverColliderNext; }

    bool GetIsClearAllGoal();
    bool GetIsClearExceptLastStage();

    void ResetStageStatus(){
        for(auto& clear : clearStatus_){
            clear = false;
        }
    }
};