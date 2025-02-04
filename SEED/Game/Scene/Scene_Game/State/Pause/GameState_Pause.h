#pragma once
#include <memory>
#include "State_Base.h"
#include "Scene_Base.h"

class Scene_Game;

// ゲームの基底ステート
class GameState_Pause : public State_Base{

public:
    GameState_Pause() = default;
    GameState_Pause(Scene_Base* pScene);
    ~GameState_Pause()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;

private:
    void EnterPause();
    void ExitPause();
    void UpdateItems();
    void UIMotion();

protected:
    Scene_Game* pGameScene_;
    int32_t selectIndex_;

    // ポーズ入り、抜けのパラメーターやフラグ
    bool isEnterPause_ = true;
    bool isExitPause_ = false;
    bool closeOrder_ = false;
    std::pair<float, const float> enterTime_ = {0.5f,0.5f};
    std::pair<float, const float> exitTime_ = { 0.5f,0.5f };

private:// ポーズ画面のUIスプライト
    std::unique_ptr<std::pair<Sprite,Sprite>> pauseMenuItems_[3];
    std::unique_ptr<Sprite> backSprite_;

private:// ポーズ管理用enum
    enum PAUSE_ITEM{
        PAUSE_ITEM_RESET = 0,
        PAUSE_ITEM_TO_SELECT,
        PAUSE_ITEM_CLOSE,
    };
};