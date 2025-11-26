#pragma once
#include <json.hpp>
#include <cstdint>
#include <Game/Objects/SongSelect/SongInfo.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

/// <summary>
/// プレイ結果を格納する構造体
/// </summary>
struct PlayResult{
    int32_t maxCombo = 0;
    int32_t totalCombo = 0;
    float score = 100.0f;
    int32_t evalutionCount[(int)Judgement::Evalution::kEvaluationCount]{};
    int32_t fastCount = 0;
    int32_t lateCount = 0;
    ScoreRank rank = ScoreRank::D;
    bool isFullCombo = false;
    bool isAllPerfect = false;
    nlohmann::json notesJson = nlohmann::json::object();
    GameObject2D* scoreTextObj = nullptr;
    TextBox2D* scoreText = nullptr;
    std::string title = "none";

public:
    void ScoreTextUpdate();

private:
    void Initialize();
};
