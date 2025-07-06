#pragma once
#include <string>
#include <cstdint>
#include <optional>
#include <json.hpp>
#include <SEED/Lib/Structs/Sprite.h>

enum class TrackDifficulty : int32_t {
    Basic = 0,
    Expert,
    Master,
    Parallel,
    kMaxDifficulty
};

enum class SongGenre : int32_t {
    Original = 0,
    GameMusic
};

enum class ScoreRank : int32_t {
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

enum class ClearIcon : int32_t {
    None = 0,
    FullCombo,
    Perfect
};

namespace GroupNameUtils{
    inline std::array<std::string, int(SongGenre::GameMusic) + 1> genreNames = {
        "オリジナル", "ゲームミュージック"
    };

    inline std::array<std::string, int(ScoreRank::kMaxRank)> rankNames = {
        "未プレイ", "D", "C", "B", "A", "S", "SS", "SSS", "ULT"
    };
}


namespace ScoreRankUtils {
    inline float scoreRankBorders[(int)ScoreRank::kMaxRank] = { 
        0.0f, 80.0f, 90.0f, 95.0f, 97.0f, 99.0f, 99.5f,100.0f
    };

    inline ScoreRank GetScoreRank(float score) {
        for (int i = 0; i < (int)ScoreRank::kMaxRank; ++i) {
            if (score <= scoreRankBorders[i]) {
                return static_cast<ScoreRank>(i);
            }
        }

        return ScoreRank::ULT; // 最後のランクを返す
    }
}

struct SongInfo{
    void Initialize(const std::string& songName);

    // 並び替え後のインデックス
    int32_t sortIndex = -1;
    int32_t groupIndex = -1;

    // 曲の共通情報
    std::string songName;
    std::string artistName;
    std::optional<SongGenre> genre;
    float bpm;

    // 難易度ごとの情報
    static const int diffcultySize = (int)TrackDifficulty::kMaxDifficulty;
    float score[diffcultySize];
    ScoreRank ranks[diffcultySize];
    ClearIcon clearIcons[diffcultySize];
    std::string notesDesignerName[diffcultySize];
    int32_t difficulty[diffcultySize];
    nlohmann::json noteDatas[diffcultySize];
};

struct SongInfoDrawer{
    static void Initialize();
    static void Draw(const SongInfo& songInfo,const Transform2D& transform,bool isSelected);
    static void Edit();

    // 画像関連
    static inline std::unique_ptr<Sprite> backSprite = nullptr;
    static inline std::unique_ptr<Sprite> jacketSprite = nullptr;
    static inline Vector2 kDrawSize;
};