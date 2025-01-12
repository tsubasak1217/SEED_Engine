#pragma once

// lib
#include <string>
#include <cstdint>

class EnemyManager;

class EnemyEditor{
public:
    explicit EnemyEditor(EnemyManager* manager);
    ~EnemyEditor() = default;

    void ShowImGui();

private:
    //--- 新規追加: セーブ/ロード機能 ---
    void SaveEnemies();
    void LoadEnemies();

private:
    EnemyManager* pEnemyManager_ = nullptr;
    uint32_t selectedEnemyIndex_ = 0;
    int32_t enemyCount_ = 0;
};
