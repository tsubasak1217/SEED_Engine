#pragma once
#include <list>
#include "SEED.h"

class Player{
public:
    Player();
    ~Player();

    void Update();
    void Draw();

public:

    void MoveLeft();
    void MoveRight();

private:

    // プレイヤーの変数
    Quad2D player_;
    float speed_ = 8.0f;
    float radius_ = 20.0f;
};