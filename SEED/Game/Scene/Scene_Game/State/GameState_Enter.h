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
class GameState_Enter : public State_Base{

public:
    GameState_Enter() = default;
    GameState_Enter(Scene_Game* pScene);
    ~GameState_Enter();

    void Update();
    void Draw();

private:

};