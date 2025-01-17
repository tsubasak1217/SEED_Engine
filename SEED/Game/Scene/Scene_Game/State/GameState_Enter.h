#pragma once
#include "State_Base.h"
#include "Scene_Base.h"

// ゲームの基底ステート
class GameState_Enter : public State_Base{

public:
    GameState_Enter() = default;
    GameState_Enter(Scene_Base* pScene);
    ~GameState_Enter()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;

private:

};