#include "Note_Tap.h"

Note_Tap::Note_Tap() : Note_Base(){
}

Note_Tap::~Note_Tap(){
}

void Note_Tap::Update(){
}

void Note_Tap::Draw(){
    Note_Base::Draw();
}

////////////////////////////////////////////////////////////////////
// タップノーツの判定
////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Tap::Judge(float dif){
    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    bool isTap = input->GetIsTap();

    // タップしていないなら、判定しない
    if(!isTap){
        return Judgement::Evaluation::MISS;
    }

    // 押したレーンを取得
    auto& tapLane = input->GetTapLane();

    // 自身のレーンと押したレーンに含まれているか
    uint32_t lane = lane_ % PlayField::kKeyCount_;

    if(tapLane.find(lane) != tapLane.end()){
        // ノーツの判定
        if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD)){
            // MISS
            return Judgement::Evaluation::MISS;

        } else if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GREAT)){
            // GOOD
            return Judgement::Evaluation::GOOD;

        } else if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::PERFECT)){
            // GREAT
            return Judgement::Evaluation::GREAT;

        } else{
            // PERFECT
            return Judgement::Evaluation::PERFECT;
        }
    }

    // 押したレーンに含まれていないなら、MISS
    return Judgement::Evaluation::MISS;
}
