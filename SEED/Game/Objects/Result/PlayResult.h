#pragma once
#include <json.hpp>
#include <cstdint>
#include <Game/Objects/SongSelect/SongInfo.h>
#include <Game/Objects/Judgement/Judgement.h>

struct PlayResult{
    int32_t maxCombo;
    int32_t score;
    int32_t evalutionCount[(int)Judgement::Evaluation::kEvaluationCount];
    int32_t fastCount;
    int32_t slowCount;
    ScoreRank rank;
    nlohmann::json songData;
};
