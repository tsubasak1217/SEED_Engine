#pragma once
#include <array>
#include <Environment/Environment.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Notes/NotesData.h>
#include <SEED/Lib/Shapes/Triangle.h>
#include <Game/Objects/Judgement/AnswerLane.h>
#include <Game/Objects/Judgement/LaneBit.h>

class PlayField{
private:
    enum PlayFieldPoint{
        TOP = 0,
        RIGHT,
        BOTTOM,
        LEFT
    };

public:
    PlayField();
    ~PlayField();
    void Initialize();
    void Update();
    void Draw();

public:
    void SetEvalution(LaneBit laneBit,UpDown layer,const Vector4& color);
    void SetLanePressed(int32_t lane, const Vector4& color);
    void SetLaneReleased(int32_t lane);

public:
    static float kPlayFieldSizeX_;// プレイフィールドの幅
    static float kPlayFieldSizeY_;// プレイフィールドの高さ
    static const int32_t kKeyCount_ = 5;// 鍵盤数
    static float kKeyWidth_;// 鍵盤の幅
    float kBorderLineZ_;// 判定ボーダーラインの位置
    float farZ_ = 100.0f;// プレイフィールドの奥行き
    float nearZ_ = 50.0f;// プレイフィールドの手前

private:
    NotesData* noteData_;// セットされている譜面

    // 描画に使用する頂点情報
    std::array<Vector2, 4> playFieldPointsScreen_;// プレイフィールドの4頂点(スクリーン座標系)
    std::array<Vector3, 4> playFieldPointsWorld_;// プレイフィールドの4頂点(ワールド座標系)
    std::array<Vector3, kKeyCount_ + 1> keyboardBorderPoints_;// 鍵盤の境界線の座標(ワールド座標系)

    // 描画用の矩形
    std::array<std::array<Triangle, kKeyCount_>,2> lane_;// レーン描画用
    std::array<std::array<AnswerLane, kKeyCount_>,2> laneAnswer_;// レーンに判定が入ったときに表示するやつ
    std::array<std::array<Triangle, kKeyCount_ + 1>,2> laneBorderLine_;// レーンの境界線
    std::array<std::array<Triangle, kKeyCount_ + 1>, 2> laneBorderLineAura_;// レーンの境界線のオーラ
};