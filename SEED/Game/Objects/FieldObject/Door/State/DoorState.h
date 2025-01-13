#pragma once

class FieldObject_Door;  // 前方宣言

class DoorState{
public:
    virtual ~DoorState() = default;
    virtual void Enter(FieldObject_Door* door) = 0;
    virtual void Update(FieldObject_Door* door, float deltaTime) = 0;
    virtual void Exit(FieldObject_Door* door) = 0;
};