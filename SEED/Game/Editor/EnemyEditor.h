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
    void LoadEnemies();

private:
    //---　マウスによるルーチンポイント追加
    void AddRoutinePointByMouse();

    //オブジェクトのインデックスを取得
    Vector3 GetObjectPositionByMouse(const std::vector<std::unique_ptr<FieldObject>>& objects);
    int32_t GetObjectIndexByMouse(std::vector<std::unique_ptr<FieldObject>>& objects);

private:
    int selectedEnemyIndex_ = 0;
    int32_t enemyCount_ = 0;

    char nameBuf_[128] = "";        // 選択中の敵の名前編集用バッファ
    int lastSelectedIndex_ = -1;    // 最後に選択された敵のインデックス

    std::string selectedRoutineName = ""; // 選択中のルーチン名
};
