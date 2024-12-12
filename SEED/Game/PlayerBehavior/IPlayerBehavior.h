#pragma once

////////////////////////////////////////////////////
//  Player の 1行動 単位
////////////////////////////////////////////////////

class Player;
class IPlayerBehavior{
public:
    IPlayerBehavior(Player* _player);
    virtual ~IPlayerBehavior();

    virtual void Update() = 0;
protected:
    // この Behavior が 動かす Player
    Player* player_ = nullptr;
};