#pragma once
#include "DoorState.h"

class OpenedState : public DoorState{
public:
    void Enter(Door* door) override;
    void Update(Door* door, float deltaTime) override;
    void Exit(Door* door) override;
};