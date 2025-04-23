#pragma once
#include <unordered_set>
#include <Game/Objects/Judgement/PlayField.h>
#include <Game/Objects/Judgement/PlayerInput.h>

// 前方宣言
class NotesData;

// 音ゲーの判定を行うクラス
class Judgement{
public:
    enum Evaluation{
        PERFECT = 0,
        GREAT,
        GOOD,
        MISS,
        kEvaluationCount
    };

private:
    Judgement();
    Judgement(const Judgement&) = delete;
    void operator=(const Judgement&) = delete;
    static Judgement* instance_;

public:
    ~Judgement();
    static Judgement* GetInstance();
    void Judge(NotesData* noteGroup,float time);
    void Draw();

public:
    float GetJudgeTime(Evaluation evaluation){ return judgeTime_[evaluation]; }
    void SetPlayFieldPtr(PlayField* playField){ pPlayField_ = playField; }

private:
    float judgeTime_[kEvaluationCount];
    PlayField* pPlayField_;// プレイフィールド
    std::array<Vector4, kEvaluationCount> judgeColor_;// 判定の色
};