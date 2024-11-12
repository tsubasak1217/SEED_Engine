#pragma once
#include "BaseEnemy.h"
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

private:
    std::list<std::unique_ptr<BaseEnemy>>obstacles_;
};