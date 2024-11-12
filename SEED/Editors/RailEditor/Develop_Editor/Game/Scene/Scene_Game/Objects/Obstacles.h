#pragma once
#include "BaseEnemy.h"
#include "Player.h"
#include <json.hpp>

class Obstacles{
    
public:
    Obstacles();
    ~Obstacles();

    void Initialize();
    void Update();
    void Draw();

private:
    
    void EditByImGui();
    void OutputToJson();
    void LoadFromJson();
    void CheckCollision();

public:

    void SetPlayerPtr(Player* pPlayer){ pPlayer_ = pPlayer; }

private:
    std::list<std::unique_ptr<BaseEnemy>>obstacles_;
    std::list<std::unique_ptr<Bullet>>particles_;

    Player* pPlayer_ = nullptr;
    float effectDistance_ = 10.0f;
};