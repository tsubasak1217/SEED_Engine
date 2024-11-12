#pragma once
#include "BaseEnemy.h"

class Ghost : public BaseEnemy{
public:
    Ghost();
    ~Ghost()override;

public:
    void Update() override;

};