#pragma once
#include "BaseEnemy.h"

class UFO : public BaseEnemy{
public:
    UFO();
    ~UFO()override;

public:
    void Update() override;

private:
    Vector3 direction_;
    float speed_ = 0.5f;
    float waitTime_ = 0.7f;
};