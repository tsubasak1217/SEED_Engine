#pragma once
#include "StageManager.h"
#include "FollowCamera.h"

class StageSelector{
public:
    StageSelector() = default;
    ~StageSelector() = default;

public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();

public:
    void SetStageManager(StageManager* stageManager){ pStageManager_ = stageManager; }
    void SetCamera(FollowCamera* camera){ pCamera_ = camera; }

private:
    StageManager* pStageManager_;
    FollowCamera* pCamera_;
};