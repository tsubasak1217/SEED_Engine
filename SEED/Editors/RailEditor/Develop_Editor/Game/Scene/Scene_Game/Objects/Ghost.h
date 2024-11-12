#pragma once
#include "BaseEnemy.h"

class Ghost : public BaseEnemy{
public:
    Ghost();
    ~Ghost()override;

public:
    void Update() override;

private:
    Vector3 direction_;
    float speed_ = 0.1f;
};