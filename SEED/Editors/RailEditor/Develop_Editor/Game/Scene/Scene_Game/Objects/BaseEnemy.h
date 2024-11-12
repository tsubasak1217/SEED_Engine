#pragma once
#include "Model.h"
#include <memory>

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

protected:
    void GetID();

public:
    std::unique_ptr<Model>model_ = nullptr;
    EnemyType type_;

    static int32_t nextObstacleID_;
    int32_t obstacleID_;
};