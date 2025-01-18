#pragma once
#include <array>
#include <memory>
#include "../../SEED/Lib/patterns/ISubject.h"
#include "../Manager/Stage.h"

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

public:
    void HandOverColliders();
    void StepStage(int32_t step);

private:
    void LoadStages();

private:
    static const int32_t kStageCount_ = 10;
    int32_t currentStageNo_ = 0;
    std::array<std::unique_ptr<Stage>, kStageCount_> stages_;

public:
    int32_t GetStageCount()const{ return kStageCount_; }
    std::array<std::unique_ptr<Stage>, kStageCount_>& GetStages(){ return stages_; }
    Stage* GetCurrentStage(){ return stages_[currentStageNo_].get(); }
    int32_t GetCurrentStageNo()const{ return currentStageNo_; }
    void SetCurrentStageNo(int32_t stageNo){ currentStageNo_ = stageNo; }
};