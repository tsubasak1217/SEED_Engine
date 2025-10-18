#pragma once
#include <SEED/Source/Basic/Scene/SceneState_Base.h>
#include <Game/Manager/RythmGameManager.h>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <memory>
#include <list>
#include <SEED/Lib/Structs/TextBox.h>

class GameState_Strolling : public SceneState_Base{
public:
    GameState_Strolling();
    GameState_Strolling(Scene_Base* pScene);
    ~GameState_Strolling() override = default;

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

};