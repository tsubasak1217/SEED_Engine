#pragma once

// state
#include "State_Base.h"
#include "Scene_Game.h"

// lib
#include <memory>

class GameState_Title : public State_Base{

public:
    GameState_Title() = default;
    GameState_Title(Scene_Base* pScene);
    ~GameState_Title()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;

private:
    Scene_Game* pGameScene_;

};

