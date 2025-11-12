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