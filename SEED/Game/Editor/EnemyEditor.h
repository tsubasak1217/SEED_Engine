#pragma once

// local 

// lib
#include <string>
#include <cstdint>

class EnemyManager;

class EnemyEditor{
public:
    explicit EnemyEditor();
    ~EnemyEditor() = default;

    void ShowImGui();

private:
    //--- 新規追加: セーブ/ロード機能 ---
    void SaveEnemies();
    void LoadEnemies();

private:
    int selectedEnemyIndex_ = 0;
    int32_t enemyCount_ = 0;

    char nameBuf_[128] = "";        // 選択中の敵の名前編集用バッファ
    int lastSelectedIndex_ = -1;    // 最後に選択された敵のインデックス
};
