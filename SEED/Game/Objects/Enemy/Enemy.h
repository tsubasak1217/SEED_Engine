#pragma once
#include <list>
#include <vector>
#include "Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "Camera.h"

class IEnemyState;

class Enemy : public BaseCharacter{
public:
    Enemy();
    ~Enemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;

};