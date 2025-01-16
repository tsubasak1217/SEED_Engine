#pragma once
#include <array>
#include <memory>
#include <Stage.h>

class StageManager{
public:
    StageManager();
    ~StageManager();

public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();

public:
    void HandOverColliders();

private:
    static const int32_t kStageCount_ = 10;
    std::array<std::unique_ptr<Stage>, kStageCount_> stages_;

public:
    int32_t GetStageCount()const{ return kStageCount_; }
    std::array<std::unique_ptr<Stage>, kStageCount_>& GetStages(){ return stages_; }
};