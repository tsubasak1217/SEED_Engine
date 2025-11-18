#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <optional>
#include <json.hpp>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Lib/Structs/Sprite.h>

// 譜面の難易度区分
enum class TrackDifficulty : int32_t{
    Basic = 0,
    Expert,
    Master,
    Parallel,
    kMaxDifficulty
};

// 曲のジャンル
enum class SongGenre : int32_t{
    Original = 0,
    GameMusic,
    kMaxGenre
};

// スコアのランク
enum class ScoreRank : int32_t{
    None = 0,
    D,
    C,
    B,
    A,
    S,
    SS,
    SSS,
    ULT,
    kMaxRank
};

// クリアアイコン
enum class ClearIcon : int32_t{
    None = 0,
    FullCombo,
    Perfect
};


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

// 選択中のもののカテゴリ
enum class SelectMode{
    Song, // 楽曲選択
    Group, // グループ選択
    Difficulty, // 難易度選択
};

// 実際の名前の定義
namespace GroupNameUtils{
    inline std::array<std::string, int(SongGenre::kMaxGenre)> genreNames = {
        "オリジナル", "ゲームミュージック"
    };

    inline std::array<std::string, int(ScoreRank::kMaxRank)> rankNames = {
        "未プレイ", "D", "C", "B", "A", "S", "SS", "SSS", "ULT"
    };
}


// 選択・ソートモードの詳細
namespace ModeUtil{
    inline std::string groupModeNames[(int)GroupMode::kMaxCount] = {
        "全曲", "ジャンル", "難易度", "ランク"
    };

    inline std::string sortModeNames[(int)SortMode::kMaxCount] = {
        "タイトル順", "アーティスト順", "レベル順", "スコア順", "クリアアイコン順", "BPM順"
    };
}

// ランクのボーダー値の定義
namespace ScoreRankUtils{
    inline float scoreRankBorders[(int)ScoreRank::kMaxRank] = {
        0.0f, 60.0f, 80.0f, 90.0f, 95.0f, 97.0f, 99.0f,100.0f
    };

    inline ScoreRank GetScoreRank(float score){
        for(int i = 1; i < (int)ScoreRank::kMaxRank; ++i){
            if(score < scoreRankBorders[i]){
                return static_cast<ScoreRank>(i);
            }
        }

        return ScoreRank::ULT; // 最後のランクを返す
    }
}