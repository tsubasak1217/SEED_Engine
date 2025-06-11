#pragma once
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <Game/Manager/RythmGameManager.h>

class GameState_Play : public State_Base{
public:
    GameState_Play();
    GameState_Play(Scene_Base* pScene);
    ~GameState_Play() override = default;

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