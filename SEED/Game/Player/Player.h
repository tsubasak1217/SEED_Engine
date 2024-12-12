#pragma once

#include <memory>

#include "../GameObject/BaseObject.h"

class IPlayerBehavior;
class Player
    : public BaseObject{
public:
    Player();
    ~Player();

    void Initialize()override;
    void Update()override;
private:
    std::unique_ptr<IPlayerBehavior> currentBehavior_;
private:
};