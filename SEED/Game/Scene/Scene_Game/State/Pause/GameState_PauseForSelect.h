#pragma once

// 基底クラス
#include "GameState_PauseBase.h"

class GameState_PauseForSelect:public GameState_PauseBase{

public:
    GameState_PauseForSelect() = default;
    GameState_PauseForSelect(Scene_Base* pScene);
    ~GameState_PauseForSelect()override = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override{};
    void EndFrame()override;
    void HandOverColliders()override{};
    void ManageState()override;

private:
    enum PAUSE_ITEM{
        PAUSE_ITEM_BACKTITLE = 0,    //< タイトルに戻る
        PAUSE_ITEM_CLOSE,            //< ポーズを終了
    };

};

