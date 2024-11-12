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
    int32_t GetPoint()const{ return point_; }

protected:
    void GetID();

public:
    std::unique_ptr<Model>model_ = nullptr;
    EnemyType type_;
    float radius_ = 2.0f;
    int32_t obstacleID_;
    
protected:
    static int32_t nextObstacleID_;
    int32_t point_ = 50;

    bool isActive_ = false;
    bool isAlive_ = true;
    float lifetime_ = 5.0f;
};