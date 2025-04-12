#pragma once
#include <array>
#include <Environment/Environment.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Notes/NotesData.h>
#include <SEED/Lib/Shapes/Quad.h>

class PlayField{
private:
    enum PlayFieldPoint{
        TOP = 0,
        RIGHT,
        BOTTOM,
        LEFT
    };

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
    float kPlayFieldSizeY_;// プレイフィールドの高さ
    static const int32_t kKeyCount_ = 5;// 鍵盤数
    float kKeyWidth_;// 鍵盤の幅
    float kBorderLineZ_;// 判定ボーダーラインの位置
    float farZ_ = 100.0f;// プレイフィールドの奥行き
    float nearZ_ = 5.0f;// プレイフィールドの手前

private:
    NotesData* noteGroup_;// セットされている譜面
    std::array<Vector2, 4> playFieldPointsScreen_;// プレイフィールドの4頂点(スクリーン座標系)
    std::array<Vector3, 4> playFieldPointsWorld_;// プレイフィールドの4頂点(ワールド座標系)
    std::array<Vector3, kKeyCount_ + 1> keyboardBorderPoints_;// 鍵盤の境界線の座標(ワールド座標系)
    std::array<Quad, kKeyCount_ * 2> keyboardBorderPointsScreen_;// レーン描画用の矩形

};