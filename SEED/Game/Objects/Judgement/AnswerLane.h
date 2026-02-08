#pragma once
#include <SEED/Lib/Shapes/Triangle.h>

/// <summary>
/// レーンを押した際のリアクションを表示する用のクラス
/// </summary>
struct AnswerLane{
public:
    void Update();
    void Draw();

public:
    SEED::Topology::Triangle tri;// 押したアンサー表示用の矩形
    SEED::Topology::Triangle evaluationPolygon;// 判定表示用のポリゴン
    inline static const float kVisibleTime = 0.2f;// 判定の表示時間
    float leftTime = 0.0f;// 判定の残り時間
    float evaluationLeftTime = 0.0f;// 判定結果描画の残り時間
    bool isTrigger = false;// 判定が入ったかどうか
    bool isPress = false;// 押しているか
    bool isRelease = false;// 判定が終わったかどうか
    bool isTapNote = false;// ノーツを処理したかどうか
    float baseScale;
};