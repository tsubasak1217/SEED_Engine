#include "Note_SideFlick.h"

Note_SideFlick::Note_SideFlick() : Note_Base(){
}

Note_SideFlick::~Note_SideFlick(){
}

void Note_SideFlick::Update(){
}

void Note_SideFlick::Draw(float currentTime, float appearLength){
    Note_Base::Draw(currentTime,appearLength);
}

Judgement::Evaluation Note_SideFlick::Judge(float dif){
    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    LR flickDirection = input->GetSideFlickDirection();
    bool isFlick = input->GetIsSideFlick();

    // フリックしていないなら、判定しない
    if(!isFlick){
        return Judgement::Evaluation::MISS;
    }

    // フリックの方向が自身のレーンと一致しているか
    if(flickDirection != flickDirection_){
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

#ifdef _DEBUG
void Note_SideFlick::Edit(){
}
#endif // _DEBUG
