#pragma once
#include <unordered_set>
#include <Game/Objects/Judgement/PlayerInput.h>

// 前方宣言
class NotesData;
class Note_Hold;
class PlayField;


/// <summary>
/// 音ゲーの判定を行うクラス
/// </summary>
class Judgement{
public:
    enum Evalution{
        PERFECT = 0,
        GREAT,
        GOOD,
        MISS,
        NONE = MISS,
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
    void Judge(NotesData* noteGroup);
    void JudgeHoldEnd(Note_Hold* note);
    void Draw();

public:
    float GetJudgeTime(Evalution evaluation){ return judgeTime_[evaluation]; }

private:
    float judgeTime_[kEvaluationCount];
    PlayField* pPlayField_ = PlayField::GetInstance();// プレイフィールド
    std::array<Vector4, kEvaluationCount> judgeColor_;// 判定の色
};