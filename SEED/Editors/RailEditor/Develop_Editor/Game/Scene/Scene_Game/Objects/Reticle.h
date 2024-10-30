#pragma once
#include "SEED.h"

class Reticle{
public:
    Reticle();
    ~Reticle();

    void Update();
    void Draw();

public:

    Vector2 GetPosition()const{ return reticle_->translate; }

private:

    float speed_ = 50.0f;
    std::unique_ptr<Sprite>reticle_ = nullptr;

};