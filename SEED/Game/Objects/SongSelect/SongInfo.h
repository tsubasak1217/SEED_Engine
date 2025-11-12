#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <optional>
#include <json.hpp>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Lib/Structs/Sprite.h>
#include "SongSelectUtils.h"


/// <summary>
/// 曲情報を格納する構造体
/// </summary>
struct SongInfo{
    void Initialize(const std::string& _folderName);

    // 曲の共通情報
    std::string folderName;
    std::string songName;
    std::string artistName;
    std::string audioFilePath;
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
    int32_t sortIndex[diffcultySize] = { -1,-1,-1,-1 };
    int32_t groupIndex[diffcultySize] = { -1,-1,-1,-1 };
};

// ペアの省略形
using Track = std::pair<SongInfo*, TrackDifficulty>;

// 楽曲グループ情報
struct SongGroup{
    uint32_t groupIdx; // グループ番号
    std::string groupName; // グループ名
    std::vector<Track> groupMembers; // グループに属する楽曲とその難易度のリスト
};

struct SongInfoDrawer{
    static void Initialize();
    static void Draw(const SongInfo& songInfo, const Transform2D& transform, TrackDifficulty difficulty, bool isSelected, float alpha = 1.0f);
    static void Draw(const SongGroup& groupInfo, const Transform2D& transform, TrackDifficulty difficulty, bool isSelected, float alpha = 1.0f);
    static void Edit();

private:
    static void SaveSettings();
    static void LoadSettings();
    static nlohmann::json ToJson();

private:
    // 画像関連
    static inline std::unique_ptr<Sprite> backSprite = nullptr;
    static inline std::unique_ptr<Sprite> jacketSprite = nullptr;
    static inline Vector2 kDrawSize;
    static inline Vector4 backColors[(int)TrackDifficulty::kMaxDifficulty];

    // テキストボックス関連
    static inline std::unordered_map<std::string, std::unique_ptr<TextBox2D>> textBox{};
    static inline std::unordered_map<std::string, Vector2>  textBoxRelativePos{};// テキストボックスの位置(相対座標)
    static inline std::vector<std::string> textBoxKeys = {
        "SongName", "GroupName","ArtistName", "BPM","NotesDesigner","Difficulty","Score"
    };
};