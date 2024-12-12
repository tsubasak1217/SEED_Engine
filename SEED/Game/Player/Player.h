#pragma once

#include <memory>

#include "../GameObject/GameObject.h"

class IPlayerBehavior;
class Player
    : public GameObject{
public:
    Player();
    ~Player();

    void Initialize()override;
    void Update()override;
private:
    std::unique_ptr<IPlayerBehavior> currentBehavior_;
private:
};