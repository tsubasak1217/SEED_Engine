#pragma once
#include "Model.h"
#include <memory>
#include "ClockManager.h"

enum class EnemyType{
    Baloon,
    Ghost,
    UFO
};

class BaseEnemy{
public:
    BaseEnemy();
    virtual ~BaseEnemy(){};

public:
    virtual void Update();
    virtual void Draw();

public:
    bool GetIsAlive()const{ return isAlive_; };
    void SetIsAlive(bool flag){ isAlive_ = flag; }

    bool GetIsActive()const{ return isActive_; }
    void SetIsActive(bool flag){ isActive_ = flag; }

protected:
    void GetID();

public:
    std::unique_ptr<Model>model_ = nullptr;
    EnemyType type_;
    float radius_ = 5.0f;
    int32_t obstacleID_;
    
protected:
    static int32_t nextObstacleID_;

    bool isActive_ = false;
    bool isAlive_ = true;
    float lifetime_ = 5.0f;
};