#pragma once
#include <memory>
#include "State_Base.h"
#include "Scene_Game.h"
#include "../Manager/StageSelector.h"

// ゲームの基底ステート
class GameState_Select : public State_Base{

public:
    GameState_Select() = default;
    GameState_Select(Scene_Base* pScene);
    ~GameState_Select()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;

private:
    Scene_Game* pGameScene_;
    std::unique_ptr<StageSelector> stageSelector_;

private:
    std::unique_ptr<Sprite> fade_ = nullptr;
    bool fadeStarted_ = false; // フェード開始フラグ
    const float fadeSpeed_ = 0.05f; // 1フレームあたりのフェード増加量
};