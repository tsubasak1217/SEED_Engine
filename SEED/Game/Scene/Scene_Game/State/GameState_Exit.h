#pragma once
#include "State_Base.h"
#include "Scene_Base.h"


// ゲームの基底ステート
class GameState_Exit : public State_Base{

public:
    GameState_Exit() = default;
    GameState_Exit(Scene_Base* pScene);
    ~GameState_Exit()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;

private:

};