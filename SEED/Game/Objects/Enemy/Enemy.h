#pragma once
#include <list>
#include <vector>
#include "Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "BaseCamera.h"
#include "Player/Player.h"

class EnemyManager;
class Enemy
    : public BaseCharacter{
public:
    /// <summary>
    /// 
    /// </summary>
    /// <param name="pManager"></param>
    /// <param name="pPlayer"></param>
    /// <param name="enemyName"> インスタンスの ID として扱う</param>
    Enemy(EnemyManager* pManager,Player* pPlayer,const std::string& enemyName);
    ~Enemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;

protected:
    void Damage(int32_t damage)override;

public:
    //--- getter / setter ---//
    float GetDistanceToPlayer()const;
    const Player* GetTargetPlayer()const{ return pPlayer_; }

    const EnemyManager* GetManager()const{ return pManager_; }

    void SetPosition(const Vector3& pos){ model_->translate_ = pos; }
    int32_t GetHP()const{ return HP_; }
    void SetHP(int32_t hp){ HP_ = hp; }

    bool GetCanEat()const{ return canEat_; }
    bool GetChasePlayer()const{ return cahsePlayer_; }

    const std::string& GetRoutineName()const{ return routineName_; }

public:
    void OnCollision(const BaseObject* other,ObjectType objectType)override;

private:
    Player* pPlayer_ = nullptr;
    EnemyManager* pManager_ = nullptr;

    std::string routineName_ = "NULL";

    bool canEat_ = false;
    bool cahsePlayer_ = false;

private:
    int32_t HP_;
};