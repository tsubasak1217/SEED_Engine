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

    // アクセサ
    const std::string& GetFolderName() const { return folderName; }
    const std::string& GetSongName() const { return songName; }
    const std::string& GetArtistName() const { return artistName; }
    const std::string& GetAudioFilePath() const { return audioFilePath; }
    const std::optional<SongGenre>& GetGenre() const { return genre; }
    float GetBpm() const { return bpm; }
    float GetSongOffsetTime() const { return songOffsetTime; }
    const Range1D& GetSongPreviewRange() const { return songPreviewRange; }
    float GetSongVolume() const { return songVolume; }
    bool IsInitialized() const { return isInitialized; }
    float GetScore(int idx) const { return score[idx]; }
    ScoreRank GetRank(int idx) const { return ranks[idx]; }
    ClearIcon GetClearIcon(int idx) const { return clearIcons[idx]; }
    const std::string& GetNotesDesignerName(int idx) const { return notesDesignerName[idx]; }
    int32_t GetDifficulty(int idx) const { return difficulty[idx]; }
    const nlohmann::json& GetNoteData(int idx) const { return noteDatas[idx]; }
    const std::string& GetJsonFilePath(int idx) const { return jsonFilePath[idx]; }

    // 曲の共通情報（後方互換性のためpublicに残す）
    std::string folderName;
    std::string songName;
    std::string artistName;
    std::string audioFilePath;
    std::optional<SongGenre> genre;
    float bpm;
    float songOffsetTime;
    Range1D songPreviewRange;
    float songVolume = 0.5f;
    bool isInitialized = false;

    // 難易度ごとの情報
    static const int diffcultySize = (int)TrackDifficulty::kMaxDifficulty;
    float score[diffcultySize];
    ScoreRank ranks[diffcultySize];
    ClearIcon clearIcons[diffcultySize];
    std::string notesDesignerName[diffcultySize];
    int32_t difficulty[diffcultySize];
    nlohmann::json noteDatas[diffcultySize];
    std::string jsonFilePath[diffcultySize];
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