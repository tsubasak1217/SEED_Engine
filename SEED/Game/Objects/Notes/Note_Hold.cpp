#include "Note_Hold.h"

Note_Hold::Note_Hold() : Note_Base(){
}

Note_Hold::~Note_Hold(){
}

void Note_Hold::Update(){
}

void Note_Hold::Draw(){
    Note_Base::Draw();
}

/////////////////////////////////////////////////////////////////////
// ホールドノーツの判定
/////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Hold::Judge(float dif){
    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    bool isHold = input->GetIsHold();
    // ホールドしていないなら、判定しない
    if(!isHold){
        return Judgement::Evaluation::MISS;
    }
    // 押したレーンを取得
    auto& holdLane = input->GetHoldLane();

    // 自身のレーンと押したレーンに含まれているか
    uint32_t lane = lane_ % PlayField::kKeyCount_;
    if(holdLane.find(lane) != holdLane.end()){

    }
    // 押したレーンに含まれていないなら、MISS
    return Judgement::Evaluation::MISS;
}
