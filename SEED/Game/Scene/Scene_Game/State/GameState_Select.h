#pragma once
#include <SEED/Source/Basic/Scene/SceneState_Base.h>
#include <Game/Manager/RythmGameManager.h>
#include <Game/Manager/SongSelector.h>
#include <Game/Objects/SongSelect/SelectBackGroundDrawer.h>
#include <SEED/Lib/Structs/VideoPlayer.h>
#include <SEED/Source/Editor/CurveEditor/CurveEditor.h>
#include <memory>

/// <summary>
/// 楽曲を選択するステート
/// </summary>
class GameState_Select : public SceneState_Base{
public:
    GameState_Select();
    GameState_Select(Scene_Base* pScene);
    ~GameState_Select() override;

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
    std::unique_ptr<SongSelector> songSelector_;
    std::unique_ptr<SelectBackGroundDrawer> backGroundDrawer_;
    GameObject2D* mouseCursorObj_ = nullptr;
};