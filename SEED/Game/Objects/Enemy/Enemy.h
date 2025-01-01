#pragma once
#include <list>
#include <vector>
#include "Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "BaseCamera.h"
#include "Player/Player.h"

class IEnemyState;

class Enemy : public BaseCharacter{
public:
    Enemy();
    Enemy(Player* pPlayer);
    ~Enemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;

public:

    float GetDistanceToPlayer()const;

private:
    Player* pPlayer_ = nullptr;
};