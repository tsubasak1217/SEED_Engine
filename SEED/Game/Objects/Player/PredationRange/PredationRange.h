#pragma once

/// stl
//memory
#include <memory>
//list
#include <list>

//object
class Player;
class Enemy;
//manager
class EnemyManager;

//math
#include "Vector3.h"

struct PreyInfomation;

/// <summary>
/// プレイヤーの 捕食可能範囲を表すクラス (範囲内のEnemyを保持する)
/// </summary>
class PredationRange{
public:
    PredationRange();
    ~PredationRange();

    void Initialize(Player* player);
    void Update(EnemyManager* _enemyManager);
    
private:
    float range_ = 0.f;
    float rangeY_ = 0.f;

    float catchAngle_ = 0.f;

    Player* player_ = nullptr;
    std::list<PreyInfomation> preyList_;
public:
    void SetRange(float range){ range_ = range; }
    void SetRangeY(float rangeY){ rangeY_ = rangeY; }
    float GetRange() const{ return range_; }
    float GetRangeY() const{ return rangeY_; }
    const std::list<PreyInfomation>& GetPreyList() const{ return preyList_; }
};

struct PreyInfomation{
    PreyInfomation(Enemy* _enemy,Vector3 _diff):enemy(_enemy),diff(_diff){}
    Enemy* enemy = nullptr;
    Vector3 diff = {};
};
using PreyInfo = PreyInfomation;