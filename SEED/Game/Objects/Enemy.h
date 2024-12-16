#pragma once
#include <list>
#include <SEED.h>
#include "ColliderObject.h"

class Enemy{
public:
    Enemy();
    ~Enemy();

    void Update();
    void Draw();

public:
    void SetIsAlive(bool isAlive){ isAlive_ = isAlive; }
    bool GetIsAlive() const{ return isAlive_; }
    const std::list<ColliderObject>& GetColliders() const{ return colliders_; }

private:
    Quad2D enemy_;
    float radius_ = 10.0f;
    bool isAlive_ = true;

    // コライダー
    std::list<ColliderObject> colliders_;
};