#include "Judgement.h"

////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////
Judgement::Judgement(){
    judgeTime_[Evaluation::PERFECT] = 3.0f / 60.0f;// パーフェクト(前後3フレーム)
    judgeTime_[Evaluation::GREAT] = 5.0f / 60.0f;// グレート(前後5フレーム)
    judgeTime_[Evaluation::GOOD] = 7.0f / 60.0f;// グッド(前後7フレーム)
}

Judgement::~Judgement(){
}


////////////////////////////////////////////////////////
// インスタンスの取得
////////////////////////////////////////////////////////
Judgement* Judgement::GetInstance(){
    if(!instance_){
        instance_ = new Judgement();
    }
    return instance_;
}


////////////////////////////////////////////////////////
// 判定
////////////////////////////////////////////////////////
void Judgement::Judge(NotesData* noteGroup, float time){
    /*--------------------------*/
    // ノーツの取得
    /*--------------------------*/
    auto nearNotes = noteGroup->GetNearNotes(time);
    if(nearNotes.empty()){ return; }

    /*--------------------------*/
    // Input情報の取得
    /*--------------------------*/
    auto input = PlayerInput::GetInstance();
    int32_t lane = input->GetLane();// 今カーソルはどこのレーンか
    LR flickDirection = input->GetFlickDirection();// フリックの方向、フリックしているか
    bool isFlick = input->GetIsFlick();// フリック判定があるか
    bool isTap = input->GetTapLane();// タップ判定があるか
    bool isHold = input->GetIsHold();// ホールド判定があるか
    std::unordered_set<int32_t> holdLane = input->GetHoldLane();// ホールドしているレーン

    /*--------------------------*/
    // ノーツの判定
    /*--------------------------*/
    for(auto& note : nearNotes){
        // 正しい時間との差を取得
        float dif = std::abs(note.lock()->time_ - time);

        // ノーツの判定
        if(note.lock()->time_ < time - judgeTime_[Evaluation::GOOD]){
            // MISS
        } else if(note.lock()->time_ < time - judgeTime_[Evaluation::GREAT]){
            // GOOD
        } else if(note.lock()->time_ < time - judgeTime_[Evaluation::PERFECT]){
            // GREAT
        } else{
            // PERFECT
        }
    }
}


////////////////////////////////////////////////////////
// 描画
////////////////////////////////////////////////////////
void Judgement::Draw(){
}
