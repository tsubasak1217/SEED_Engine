#pragma once
#include <Game/Objects/SongSelect/SongInfo.h>

/// <summary>
/// セレクトシーンの背景を描画するクラス
/// </summary>
class SelectBackGroundDrawer{
public:
    SelectBackGroundDrawer();
    ~SelectBackGroundDrawer() = default;
    // 初期化
    void Initialize();
    // 更新
    void Update();
    // 描画
    void Draw();

public:
    bool isGrooveStart_ = false;
    inline static TrackDifficulty currentDifficulty; // 現在の難易度

private:
    // スクロールする背景の描画
    void DrawScrollingBackground();
    // リズムに乗って動くものの描画
    void DrawGrooveObjects();
};