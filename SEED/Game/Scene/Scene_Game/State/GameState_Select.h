#pragma once
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <Game/Manager/RythmGameManager.h>
#include <Game/Manager/SongSelector.h>
#include <Game/Objects/SongSelect/SelectBackGroundDrawer.h>
#include <memory>

class GameState_Select : public State_Base{
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
    std::unique_ptr< SelectBackGroundDrawer> backGroundDrawer_;
};