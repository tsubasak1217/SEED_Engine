#pragma once

// 基底クラス
#include "GameState_PauseBase.h"

class GameState_PauseForPlay:public GameState_PauseBase{

public:
    GameState_PauseForPlay() = default;
    GameState_PauseForPlay(Scene_Base* pScene);
    ~GameState_PauseForPlay()override = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override{};
    void EndFrame()override;
    void HandOverColliders()override{};
    void ManageState()override;

private:
    void EnterPause();
    void ExitPause();
    void UpdateItems();
    void UIMotion();

private:

private:
    enum PAUSE_ITEM{
        PAUSE_ITEM_RESET = 0,
        PAUSE_ITEM_TO_SELECT,
        PAUSE_ITEM_CLOSE,
    };
};

