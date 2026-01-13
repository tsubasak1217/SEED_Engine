#pragma once
#include <SEED/Source/Basic/Scene/SceneState_Base.h>
#include <Game/Manager/RhythmGameManager.h>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <memory>
#include <list>
#include <SEED/Lib/Structs/TextBox.h>


/// <summary>
/// プレイシーンでのポーズ状態
/// </summary>
class GameState_Pause : public SceneState_Base{
public:
    GameState_Pause();
    GameState_Pause(Scene_Base* pScene);
    ~GameState_Pause() override;

public:
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;
    void ManageState() override;

private:
    void SelectMenuItem();

private:

    enum PauseMenuItem{
        Resume,
        Retry,
        ToSelect,
        PauseMenuItemCount
    };

private:
    // 読み込んだものを格納するポインタ
    GameObject2D* pauseItemsParent_;
    std::array<GameObject2D*, PauseMenuItemCount> menus_;
    GameObject2D* backSpriteObj_ = nullptr;
    GameObject2D* mouseColliderObj_ = nullptr;

    //選択項目
    int32_t selectedIndex_ = 0;

    // 入力
    InputHandler<int32_t> selectInput_;
    InputHandler<bool> decideInput_;
    InputHandler<bool> backInput_;

    // タイマー関連
    Timer pauseTimer_;
    std::array<Timer, PauseMenuItemCount> scalingTimers_;

    // state遷移用
    bool isExit_ = false;
    bool isItemChanged_ = false;
};