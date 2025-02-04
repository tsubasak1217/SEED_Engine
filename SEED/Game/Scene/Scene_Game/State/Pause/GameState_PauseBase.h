#pragma once

// state
#include "Scene_Base.h"
#include "State_Base.h"

// lib
#include <memory>

class Scene_Game;

class GameState_PauseBase :public State_Base{

public:
    GameState_PauseBase() = default;
    GameState_PauseBase(Scene_Base* pScene);
    ~GameState_PauseBase()override = default;
    void Initialize()override;
    void Finalize()override{};
    void Update()override;
    void Draw()override;
    void BeginFrame()override{};
    void EndFrame()override;
    void HandOverColliders()override;
    virtual void ManageState()override = 0;

protected:
    void EnterPause();
    void ExitPause();

    // ポーズ画面の項目の更新
    virtual void UpdateItems() = 0;
    virtual void UIMotion() = 0;

protected:
    Scene_Game* pGameScene_;
    int32_t selectIndex_;

    // ポーズ入り、抜けのパラメーターやフラグ
    bool isEnterPause_ = true;
    bool isExitPause_ = false;
    bool closeOrder_ = false;
    std::pair<float, const float> enterTime_ = {0.5f,0.5f};
    std::pair<float, const float> exitTime_ = {0.5f,0.5f};

protected:// ポーズ画面のUIスプライト
    std::unique_ptr<Sprite> backSprite_;
    std::vector<std::unique_ptr<std::pair<Sprite, Sprite>>> pauseMenuItems_;
};

