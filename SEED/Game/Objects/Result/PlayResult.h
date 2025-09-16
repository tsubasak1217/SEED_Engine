#pragma once
#include <json.hpp>
#include <cstdint>
#include <Game/Objects/SongSelect/SongInfo.h>
#include <Game/Objects/Judgement/Judgement.h>

struct PlayResult{
    int32_t maxCombo = 0;
    int32_t totalCombo = 0;
    float score = 0.0f;
    int32_t evalutionCount[(int)Judgement::Evaluation::kEvaluationCount]{};
    int32_t fastCount = 0;
    int32_t lateCount = 0;
    ScoreRank rank;
    nlohmann::json songData;
};
