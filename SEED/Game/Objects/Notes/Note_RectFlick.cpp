#include "Note_RectFlick.h"

Note_RectFlick::Note_RectFlick() : Note_Base(){
}

Note_RectFlick::~Note_RectFlick(){
}

void Note_RectFlick::Update(){
}

void Note_RectFlick::Draw(float currentTime, float appearLength){
    Note_Base::Draw(currentTime,appearLength);
}

Judgement::Evaluation Note_RectFlick::Judge(float dif){
    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    LR flickDirection = input->GetSideFlickDirection();
    bool isFlick = input->GetIsSideFlick();

    // フリックしていないなら、判定しない
    if(!isFlick){
        return Judgement::Evaluation::MISS;
    }

    // フリックの方向が自身のレーンと一致しているか
    if(flickDirection != flickDirection){
        return Judgement::Evaluation::MISS;
    }

    // ノーツの判定(判定はゆるめにしてパーフェクトかミスだけにする)
    if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD)){
        // MISS
        return Judgement::Evaluation::MISS;
    } else{
        // PERFECT
        return Judgement::Evaluation::PERFECT;
    }
}
