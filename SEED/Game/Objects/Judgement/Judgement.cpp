#include "Judgement.h"
#include <Game/Objects/Notes/NotesData.h>
#include <Game/Objects/Judgement/PlayField.h>
#include <Game/Objects/Notes/Note_Hold.h>

// setting
#include <Game/Config/PlaySettings.h>

// managerのインクルード
#include <Game/Manager/RythmGameManager.h>

/////////////////////////////////////////////////////////
// static変数の初期化
/////////////////////////////////////////////////////////
Judgement* Judgement::instance_ = nullptr;

////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////
Judgement::Judgement(){
    // 判定の幅の設定
    judgeTime_[Evaluation::PERFECT] = 5.0f / 60.0f;// パーフェクト(前後3フレーム)
    judgeTime_[Evaluation::GREAT] = 8.0f / 60.0f;// グレート(前後5フレーム)
    judgeTime_[Evaluation::GOOD] = 12.0f / 60.0f;// グッド(前後7フレーム)

    // 判定の色の設定
    judgeColor_[Evaluation::PERFECT] = { 1.0f, 1.0f, 0.0f, 1.0f };// 黄色 
    judgeColor_[Evaluation::GREAT] = { 1.0f, 0.0f, 1.0f, 1.0f };// ピンク
    judgeColor_[Evaluation::GOOD] = { 0.0f, 1.0f, 1.0f, 1.0f };// 水色
    judgeColor_[Evaluation::MISS] = { 1.0f, 1.0f, 1.0f, 1.0f };// 白
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
void Judgement::Judge(NotesData* noteGroup){

    /*--------------------------*/
    // 押下状態はすべて設定する
    /*--------------------------*/
    std::unordered_set<int32_t> releaseLane = PlayerInput::GetInstance()->GetUnTapLane();
    std::unordered_set<int32_t> tapLane = PlayerInput::GetInstance()->GetTapLane();

    // pressの設定
    for(auto& lane : tapLane){
        pPlayField_->SetLanePressed(lane, { 1.0f,1.0f,1.0f,1.0f });
    }

    // releaseの設定
    for(auto& lane : releaseLane){
        pPlayField_->SetLaneReleased(lane);
    }

    /*--------------------------*/
    // 付近のノーツの取得
    /*--------------------------*/
    float time = noteGroup->GetCurMusicTime();
    auto nearNotes = noteGroup->GetNearNotes(time);
    if(nearNotes.empty()){ return; }

    /*--------------------------*/
    // ノーツの判定
    /*--------------------------*/
    std::list<std::pair<std::weak_ptr<Note_Base>, Judgement::Evaluation>> hitNotes;// 判定を拾ったノーツ一覧
    for(auto& note : nearNotes){
        // 正しい時間との差を取得
        float dif = std::abs((note.lock()->time_ + PlaySettings::GetInstance()->GetOffsetJudge())-time);

        // 判定を拾ったノーツをリストにする
        Judgement::Evaluation evaluation = note.lock()->Judge(dif);
        if(evaluation != Evaluation::MISS){
            hitNotes.push_back({ note,evaluation });
        }
    }

    /*--------------------------*/
    // 重複判定の解決などを行う
    /*--------------------------*/
    uint32_t hitBits = 0;
    for(auto& note : hitNotes){

        Note_Base* notePtr = note.first.lock().get();

        // すでにビットが立っているなら、重複しているのでスルー
        if(hitBits & notePtr->laneBit_){
            continue;
        } else{
            if(notePtr->noteType_ != NoteType::Hold){
                notePtr->isEnd_ = true;// ノーツを終了させる
            }
            hitBits |= notePtr->laneBit_;// ビットを立てる
            pPlayField_->SetEvalution(notePtr->laneBit_, notePtr->layer_, judgeColor_[note.second]);// レーンを押下状態にする

            // エフェクトを出す
            pPlayField_->EmitEffect(notePtr->laneBit_, notePtr->layer_, (int)note.second);

            // コンボを加算
            RythmGameManager::GetInstance()->AddCombo();
        }
    }
}

// ホ－ルドの終点を判定する
void Judgement::JudgeHoldEnd(Note_Hold* note){

    // ホールドノーツの終点を判定する
    Evaluation evaluation = note->JudgeHoldEnd();

    // コンボの管理
    if(evaluation != Evaluation::MISS){
        RythmGameManager::GetInstance()->AddCombo();// コンボを加算
    } else{
        RythmGameManager::GetInstance()->BreakCombo();// コンボを切る
    }

    // 終点の判定に応じてエフェクトとか出す
    pPlayField_->SetEvalution(note->laneBit_, note->layer_, judgeColor_[(int)evaluation]);// レーンを押下状態にする
}


////////////////////////////////////////////////////////
// 描画
////////////////////////////////////////////////////////
void Judgement::Draw(){
}
