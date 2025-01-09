#pragma once

#include <string>

class EnemyManager;

class EnemyEditor{
public:
    EnemyEditor(EnemyManager* manager);
    ~EnemyEditor() = default;

    void ShowImGui();

private:
    void LoadFromJson();
    void SaveToJson();

private:
    const std::string jsonPath = "resources/jsons/Enemy.json";

    EnemyManager* pEnemyManager_ = nullptr;
};

