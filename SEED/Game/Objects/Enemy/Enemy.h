#pragma once
#include <list>
#include <vector>
#include "Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "BaseCamera.h"
#include "Player/Player.h"

class Enemy : public BaseCharacter{
public:
    Enemy();
    Enemy(Player* pPlayer);
    ~Enemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;

protected:
    void Damage(int32_t damage)override;

public:
    float GetDistanceToPlayer()const;
    const Player* GetTargetPlayer()const{ return pPlayer_; }

public:
    void OnCollision(const BaseObject* other, ObjectType objectType)override;

private:
    Player* pPlayer_ = nullptr;

private:
    int32_t HP_;
};