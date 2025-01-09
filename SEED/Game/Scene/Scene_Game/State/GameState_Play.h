#pragma once
// external
#include <string>
#include <vector>
#include <memory>
#include <list>

// local
#include <Triangle.h>
#include <Model.h>
#include <State_Base.h>
#include <Scene_Base.h>

class GameState_Play : public State_Base{
public:
    GameState_Play() = default;
    GameState_Play(Scene_Base* pScene);
    ~GameState_Play();
    void Update()override;
    void Draw()override;
    void Initialize()override;
    void Finalize()override;
    void BeginFrame()override;
    void EndFrame()override;

private:

};