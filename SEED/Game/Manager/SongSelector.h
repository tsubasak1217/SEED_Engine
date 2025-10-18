#pragma once
#include <memory>
#include <list>
#include <SEED/Lib/Structs/TextBox.h>
#include <Game/Objects/SongSelect/SongInfo.h>
#include <SEED/Lib/Functions/Easing.h>
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Source/Basic/Camera/BaseCamera.h>

// ソート方法
enum class SortMode{
    Title,      // タイトル順
    Artist,     // アーティスト順
    Difficulty, // 難易度順
    Score,      // スコア順
    ClearIcon,  // クリアアイコン順(FC,APなど)
    BPM,        // BPM順
    //ここまで
    kMaxCount
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

namespace SelectUtil{
    inline std::string groupModeNames[(int)GroupMode::kMaxCount] = {
        "全曲", "ジャンル", "難易度", "ランク"
    };

    inline std::string sortModeNames[(int)SortMode::kMaxCount] = {
        "タイトル", "アーティスト名", "難易度", "スコア", "クリアアイコン", "BPM"
    };
}

// 楽曲選択の管理クラス
class SongSelector{
public:
    SongSelector();
    ~SongSelector();
    void Initialize();
    void Update();
    void Draw();
    void EndFrame();

public:
    void Sort(); // ソート

private:
    void CreateGroup();// グループ分け
    void SortInGroup();// グループ内ソート
    void UpdateIndex(); // インデックス更新
    void UpdateVisibleItems(); // 表示されている楽曲の更新
    void SelectItems();
    void UpdateSongUI();
    void DisplaySongDetail(); // 曲決定の際のUIがはけて行く動き
    void CreateDifficultyList(); // 難易度のリストを作成
    void UpdateDifficultyList(); // 難易度のリストを更新
    void StartPlay(); // プレイ開始
    void CameraControl();
    void UpdateJacket(); // ジャケットの更新

    void Edit();
    nlohmann::json ToJson();
    void FromJson(const nlohmann::json& json);

private:
    SelectMode selectMode_;// 楽曲選択中かどうか
    bool changeSceneOrder_ = false;
    std::list<std::unique_ptr<SongInfo>> songList; // 楽曲情報のリスト
    std::list<SongGroup> songGroups; // 楽曲グループのリスト
    int32_t currentGroupIndex = 0; // 現在のグループインデックス
    int32_t currentSongIndex = 0; // 現在の楽曲インデックス
    TrackDifficulty currentDifficulty; // 現在の難易度
    SongGroup* currentGroup = nullptr; // 現在選択中のグループ
    GroupMode currentGroupMode; // グループ分け方法
    SortMode currentSortMode; // ソートモード
    SongAndDiffidulty currentSong; // 現在選択中の楽曲

    // 表示される楽曲の数
    static const int32_t kVisibleRadius = 4; // 表示する楽曲の数
    static const int32_t centerIndex = kVisibleRadius; // 中心の楽曲インデックス
    static const int32_t kVisibleSongCount = 1 + kVisibleRadius * 2; // 表示する楽曲の総数

    // 表示曲・グループのリスト
    std::array<SongAndDiffidulty*, kVisibleSongCount> visibleSongs; // 表示されている楽曲のリスト
    std::array<SongGroup*, kVisibleSongCount> visibleGroups; // 表示されている楽曲のリスト
    std::array<Transform2D, kVisibleSongCount> visibleItemTransforms; // 表示されている楽曲の位置の制御点

    // 楽曲の難易度のリスト
    std::vector<SongAndDiffidulty> songAllDifficulty; // 楽曲の難易度のリスト
    std::vector<Transform2D> songAllDifficultyTransforms; // 楽曲の難易度の位置の制御点
    std::vector<int32_t> aimIndices; // 楽曲の難易度の位置の制御点のインデックス
    std::array<Transform2D, int(TrackDifficulty::kMaxDifficulty) + 3> difficultyAimTransforms; // 難易度の位置の制御点
    int32_t difficultyTransformCenterIdx = 3;
    float difficultySelectTimer = 0.0f; // 難易度選択のタイマー

    // UI関連
    bool isItemChanged = false; // アイテムが変更されたかどうか
    UpDown changeDirection; // アイテム変更の方向
    float itemChangeTimer = 0.0f; // アイテム変更タイマー
    float kItemChangeTime = 0.9f; // アイテム変更時間
    std::array<Transform2D, kVisibleSongCount + 2> itemAimTransforms; // アイテムの位置の制御点
    bool isEditSongTransform = false; // ベジェ曲線の表示フラグ
    bool isEditDifficultyTransform = false; // 難易度のベジェ曲線の表示フラグ
    bool isDecideSong = false; // 楽曲決定フラグ
    // 曲決定の際のUIがはけて行く動き
    float slideOutWidth = 800.0f; // スライドアウト(曲決定の際のUIがはけて行く動き)の幅
    float slideOutTimer = 0.0f;
    float kMaxSlideOutTime = 0.8f; // スライドアウトの最大時間
    bool isStartPlay = false; // プレイ開始フラグ
    float playWaitTimer = 0.0f; // プレイ待機タイマー
    float kPlayWaitTime = 2.0f; // プレイ待機時間
    // 音声
    AudioHandle songHandle_;

    // 3Dのもの
    Transform cameraControlTransforms_[4]; // カメラ制御用のトランスフォーム
    Transform preCameraControlTransform_;
    BaseCamera* camera_ = nullptr; // カメラ
    Quad jacket3D_;

};