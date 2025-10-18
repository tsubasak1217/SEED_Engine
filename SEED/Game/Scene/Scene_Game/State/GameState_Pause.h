#pragma once
#include <SEED/Source/Basic/Scene/SceneState_Base.h>
#include <Game/Manager/RythmGameManager.h>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <memory>
#include <list>
#include <SEED/Lib/Structs/TextBox.h>

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
    std::vector<GameObject2D*> pauseUIs_;
    Sprite pauseBg_;
    int32_t selectedIndex_ = 0;

    // タイマー関連
    Timer pauseTimer_;

    // state遷移用
    bool isExit_ = false;
};