#pragma once
#include "BaseEnemy.h"

class UFO : public BaseEnemy{
public:
    UFO();
    ~UFO()override;

public:
    void Update() override;

};