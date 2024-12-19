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
#include <Sprite.h>
#include <Player.h>
#include "Commands/ICommand.h"
#include "Commands/InputHandler.h"

class Scene_Game;
class GameState_Exit;
class GameState_Enter;


class GameState_Play : public State_Base{

    friend GameState_Exit;
    friend GameState_Enter;

public:
    GameState_Play(Scene_Game* pScene);
    ~GameState_Play();
    void Update()override;
    void Draw()override;
    void Initialize();

private:

    ICommand* command_;
    InputHandler* inputHandler_;
    std::unique_ptr<Player> player_;
};