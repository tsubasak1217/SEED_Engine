#pragma once
#include "BaseEnemy.h"

class Baloon : public BaseEnemy{
public:
    Baloon();
    ~Baloon()override;

public:
    void Update() override;
    
private:
    float upSpeed_ = 0.05f;
};