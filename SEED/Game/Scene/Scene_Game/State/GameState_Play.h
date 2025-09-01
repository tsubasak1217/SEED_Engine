#pragma once
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <json.hpp>

class GameState_Play : public State_Base{
public:
    GameState_Play() = default;
    GameState_Play(Scene_Base* pScene);
    ~GameState_Play() override;

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