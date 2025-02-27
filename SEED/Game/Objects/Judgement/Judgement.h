#pragma once
#include <unordered_set>
#include <Game/Objects/Judgement/PlayField.h>
#include <Game/Objects/Notes/NotesData.h>
#include <Game/Objects/Judgement/PlayerInput.h>

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

private:
    float judgeTime_[kEvaluationCount];
};