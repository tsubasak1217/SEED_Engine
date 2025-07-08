#pragma once
#include <memory>
#include <list>
#include <SEED/Lib/Structs/TextBox.h>
#include <Game/Objects/SongSelect/SongInfo.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>
#include <SEED/Lib/enums/Direction.h>

// ソート方法
enum class SortMode{
    Title,      // タイトル順
    Artist,     // アーティスト順
    Difficulty, // 難易度順
    Score,      // スコア順
    ClearIcon,  // クリアアイコン順(FC,APなど)
    BPM,        // BPM順
};

// まとまり分け方法
enum class GroupMode{
    None,
    Genre,     // ジャンルごと
    Difficulty, // 難易度ごと
    Rank,
    // ここまで
    kMaxCount
};

enum class SelectMode{
    Song, // 楽曲選択
    Group, // グループ選択
};

// 楽曲選択の管理クラス
class SongSelector{
public:
    SongSelector();
    ~SongSelector();
    void Initialize();
    void Update();
    void Draw();

public:
    void Sort(); // ソート

private:
    void CreateGroup();// グループ分け
    void SortInGroup();// グループ内ソート
    void UpdateIndex(); // インデックス更新
    void UpdateVisibleItems(); // 表示されている楽曲の更新
    void SelectItems();
    void UpdateSongUI();

    void Edit();
    nlohmann::json ToJson();
    void FromJson(const nlohmann::json& json);

private:
    SelectMode selectMode_;// 楽曲選択中かどうか
    std::list<std::unique_ptr<SongInfo>> songList; // 楽曲情報のリスト
    int32_t currentGroupIndex = 0; // 現在のグループインデックス
    int32_t currentSongIndex = 0; // 現在の楽曲インデックス
    TrackDifficulty currentDifficulty; // 現在の難易度
    SongGroup* currentGroup = nullptr; // 現在選択中のグループ
    SongInfo* currentSong = nullptr; // 現在選択中の楽曲

    std::list<SongGroup> songGroups; // 楽曲グループのリスト
    static const int32_t kVisibleRadius = 4; // 表示する楽曲の数
    static const int32_t centerIndex = kVisibleRadius; // 中心の楽曲インデックス
    static const int32_t kVisibleSongCount = 1 + kVisibleRadius * 2; // 表示する楽曲の総数
    std::array<SongInfo*, kVisibleSongCount> visibleSongs; // 表示されている楽曲のリスト
    std::array<SongGroup*, kVisibleSongCount> visibleGroups; // 表示されている楽曲のリスト
    std::array<Transform2D, kVisibleSongCount> visibleItemTransforms; // 表示されている楽曲の位置の制御点
    GroupMode currentGroupMode; // グループ分け方法
    SortMode currentSortMode; // ソートモード

    // UI関連
    bool isItemChanged = false; // アイテムが変更されたかどうか
    UpDown changeDirection; // アイテム変更の方向
    float itemChangeTimer = 0.0f; // アイテム変更タイマー
    float kItemChangeTime = 0.2f; // アイテム変更時間
    std::array<Transform2D, kVisibleSongCount + 2> itemAimTransforms; // アイテムの位置の制御点
    bool splineCurveVisible = true; // ベジェ曲線の表示フラグ
};