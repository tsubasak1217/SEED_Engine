#pragma once
#include <Game/Objects/SongSelect/SongInfo.h>
#include <SEED/Source/Basic/Object/GameObject.h>

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
    inline static bool isGrooveStart_ = false;
    inline static TrackDifficulty currentDifficulty; // 現在の難易度

private:
    GameObject* bgColorListObj_ = nullptr;
    std::array<Color, (int)TrackDifficulty::kMaxDifficulty> clearColors_;

private:
    // スクロールする背景の描画
    void DrawScrollingBackground();
    // リズムに乗って動くものの描画
    void DrawGrooveObjects();
};