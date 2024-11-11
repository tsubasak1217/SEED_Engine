#pragma once

// external
#include <string>
#include <vector>
#include <memory>
#include <list>

// local
#include <Triangle.h>
#include <Model.h>
#include "State_Base.h"
#include <Sprite.h>


class Scene_Game;
class SEED;

// ゲームの基底ステート
class GameState_Exit : public State_Base{

public:
    GameState_Exit() = default;
    GameState_Exit(Scene_Game* pScene);
    ~GameState_Exit();

    void Update();
    void Draw();

private:

};