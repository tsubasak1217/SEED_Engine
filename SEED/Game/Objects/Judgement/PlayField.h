#pragma once
#include <Environment/Environment.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Notes/NoteGroup.h>

class PlayField{
private:
    PlayField();
    PlayField(const PlayField&) = delete;
    void operator=(const PlayField&) = delete;
    static PlayField* instance_;

public:
    ~PlayField();
    static PlayField* GetInstance();
    void Update();
    void Draw();

public:
    float kPlayFieldSizeX_;// プレイフィールドの幅
    float kPlayFieldSizeZ_;// プレイフィールドの高さ
    int kKeyCount_ = 4;// 鍵盤数
    float kKeyWidth_;// 鍵盤の幅
    float kBorderLineZ_;// 判定ボーダーラインの位置

private:
    NotesData* noteGroup_;// セットされている譜面
};