#pragma once

// state
#include "State_Base.h"
#include "Scene_Game.h"

// stage
#include "Stage.h"

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

    Stage* stage_;

    Vector3 cameraOffset_;
    Vector3 cameraRotate_;

    Vector3 playerRotate_;

};

