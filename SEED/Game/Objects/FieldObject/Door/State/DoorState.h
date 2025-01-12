#pragma once

class Door;  // 前方宣言

class DoorState{
public:
    virtual ~DoorState() = default;
    virtual void Enter(Door* door) = 0;
    virtual void Update(Door* door, float deltaTime) = 0;
    virtual void Exit(Door* door) = 0;
};