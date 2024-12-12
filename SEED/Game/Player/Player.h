#pragma once

#include <memory>

#include "../GameObject/BaseObject.h"


class Player
    : public BaseObject{
public:
    Player();
    ~Player();

    void Initialize()override;
    void Update()override;
private:

private:
};