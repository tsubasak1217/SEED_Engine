#include "Note_Hold.h"

Note_Hold::Note_Hold() : Note_Base(){
}

Note_Hold::~Note_Hold(){
}

void Note_Hold::Update(){
}

void Note_Hold::Draw(float currentTime, float appearLength){
    Note_Base::Draw(currentTime, appearLength);
}

/////////////////////////////////////////////////////////////////////
// ホールドノーツの判定
/////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Hold::Judge(float dif){
    // 入力情報を取得
    uint32_t lane = lane_ % PlayField::kKeyCount_;
    static auto input = PlayerInput::GetInstance();

    // 判定時間の取得
    static float judgeTime[Judgement::kEvaluationCount] = {
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::PERFECT),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GREAT),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::MISS)
    };

    // 自身のレーンが押されているか
    isHold_ = input->GetIsPress(lane);

    // 頭をまだ押していない場合の判定
    if(headEvaluation_ == Judgement::Evaluation::NONE){

        // ホールドしていないなら、判定しない
        if(!isHold_){
            headEvaluation_ = Judgement::Evaluation::MISS;
            return headEvaluation_;

        } else{// ホールドしている


            // ノーツの判定
            if(dif > judgeTime[Judgement::Evaluation::GOOD]){
                // MISS
                releaseTime_ = judgeTime[Judgement::Evaluation::GOOD];
                return headEvaluation_ = Judgement::Evaluation::MISS;

            } else if(dif > judgeTime[Judgement::Evaluation::GREAT]){
                // GOOD
                releaseTime_ = judgeTime[Judgement::Evaluation::GREAT];
                return headEvaluation_ = Judgement::Evaluation::GOOD;

            } else if(dif > judgeTime[Judgement::Evaluation::PERFECT]){
                // GREAT
                releaseTime_ = judgeTime[Judgement::Evaluation::PERFECT];
                return headEvaluation_ = Judgement::Evaluation::GREAT;

            } else{
                // PERFECT
                releaseTime_ = 0.0f;
                return headEvaluation_ = Judgement::Evaluation::PERFECT;
            }
        }

    } else{// 頭をもう押している場合

        if(isHold_){
            return headEvaluation_;
        } else{
            // 離している時間を加算
            releaseTime_ += ClockManager::DeltaTime();
            return Judgement::Evaluation::MISS;
        }
    }
}

/////////////////////////////////////////////////////////////////////
// ホールドノーツの終点の判定
/////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Hold::JudgeHoldEnd(){

    // 判定時間の取得
    static float judgeTime[Judgement::kEvaluationCount] = {
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::PERFECT),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GREAT),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::MISS)
    };

    // 押していないないなら、ミス
    if(headEvaluation_ == Judgement::Evaluation::NONE){
        return Judgement::Evaluation::MISS;
    }

    // 押していなかった時間に応じて判定を行う(離す判定はゆるく設けるため、少しでも押していたら最低でもGOOD判定)
    if(releaseTime_ < judgeTime[Judgement::Evaluation::PERFECT]){
        return Judgement::Evaluation::PERFECT;
    } else if(releaseTime_ < judgeTime[Judgement::Evaluation::GREAT]){
        return Judgement::Evaluation::GREAT;
    } else {
        return Judgement::Evaluation::GOOD;
    }
}
