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
    static void StepStage(int32_t step);

private:
    void LoadStages();

private:
    static const int32_t kStageCount_ = 10;
    static int32_t currentStageNo_;
    static int32_t preStageNo_;
    static std::array<std::unique_ptr<Stage>,kStageCount_> stages_;
    static bool isPlaying_;

public:
    int32_t GetStageCount()const{ return kStageCount_; }
    std::array<std::unique_ptr<Stage>,kStageCount_>& GetStages(){ return stages_; }
    Stage* GetCurrentStage(){ return stages_[currentStageNo_].get(); }
    Stage* GetPreStage(){ return stages_[preStageNo_].get(); }
    static int32_t GetCurrentStageNo(){ return currentStageNo_; }
    static void SetCurrentStageNo(int32_t stageNo){ currentStageNo_ = stageNo; }
    static Vector3 GetStartPos();
    static bool IsStageChanged(){ return currentStageNo_ != preStageNo_; }
    static bool IsPlaying(){ return isPlaying_; }
};