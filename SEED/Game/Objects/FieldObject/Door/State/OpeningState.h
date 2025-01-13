#pragma once
#include "DoorState.h"

class OpeningState 
    : public DoorState{
public:
    void Enter(FieldObject_Door* door) override;
    void Update(FieldObject_Door* door, float deltaTime) override;
    void Exit(FieldObject_Door* door) override;
};