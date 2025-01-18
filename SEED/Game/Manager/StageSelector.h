#pragma once
#include "StageManager.h"
#include "FollowCamera.h"
#include "Sprite.h"

class StageSelector{
public:
    StageSelector() = default;
    StageSelector(StageManager* stageManager, FollowCamera* camera);
    ~StageSelector() = default;

public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();

private:
    void Select();
    void CameraUpdate();
    void DecideStage();
    void UpdateItems();

public:
    void SetStageManager(StageManager* stageManager){ pStageManager_ = stageManager; }
    void SetCamera(FollowCamera* camera){ pCamera_ = camera; }
    bool GetIsDecided(){ return isDecided_; }

private:
    StageManager* pStageManager_;
    FollowCamera* pCamera_;

private:
    // sprite
    std::unique_ptr<Sprite> frame_ = nullptr;
    std::unique_ptr<Sprite> stageName_ = nullptr;
    std::unique_ptr<Sprite> arrow_[2];
    std::unique_ptr<Sprite> collectionStars_[3];
    std::unique_ptr<Sprite> difficultyEggs_[5];

private:
    bool isDecided_ = false;
};