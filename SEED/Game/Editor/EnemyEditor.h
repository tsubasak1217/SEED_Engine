#pragma once

// local 
#include "Vector3.h"
#include "FieldObject/FieldObject.h"

// lib
#include <string>
#include <cstdint>
#include <vector>
#include <memory>

class EnemyManager;

class EnemyEditor{
public:
    explicit EnemyEditor();
    ~EnemyEditor() = default;

    void ShowImGui();

private:
    //--- 新規追加: セーブ/ロード機能 ---
    void SaveEnemies();

private:
    void AddRoutinePointByMouse();

private:
    int selectedEnemyIndex_ = 0;

    char nameBuf_[128] = "";        // 選択中の敵の名前編集用バッファ
    int lastSelectedIndex_ = -1;    // 最後に選択された敵のインデックス

    std::string selectedRoutineName = ""; // 選択中のルーチン名
};
