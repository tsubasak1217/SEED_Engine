#pragma once
#include "SEED.h"
#include <memory>

class Bullet{
public:

    Bullet();
    Bullet(const Vector3& position, const Vector3& direction);
    ~Bullet();

    void Update();
    void Draw();

public:
    bool GetIsAlive()const{ return lifetime_ > 0.0f; };

private:
    float lifetime_ = 5.0f;
    Vector3 direction_;
    float speed_ = 10.0f;
    std::unique_ptr<Model>bullet_ = nullptr;
};