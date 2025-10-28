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
    judgeTime_[Evalution::PERFECT] = 5.0f / 60.0f;// パーフェクト(前後5フレーム)
    judgeTime_[Evalution::GREAT] = 8.0f / 60.0f;// グレート(前後8フレーム)
    judgeTime_[Evalution::GOOD] = 12.0f / 60.0f;// グッド(前後12フレーム)

    // 判定の色の設定
    judgeColor_[Evalution::PERFECT] = { 1.0f, 1.0f, 0.0f, 1.0f };// 黄色 
    judgeColor_[Evalution::GREAT] = { 1.0f, 0.0f, 1.0f, 1.0f };// ピンク
    judgeColor_[Evalution::GOOD] = { 0.0f, 1.0f, 1.0f, 1.0f };// 水色
    judgeColor_[Evalution::MISS] = { 1.0f, 1.0f, 1.0f, 1.0f };// 白
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
    struct NoteJudgeInfo{
        std::weak_ptr<Note_Base> note;
        float signedDif; // 正しい時間との差
        float dif; // 差の絶対値
        Judgement::Evalution evaluation; // 判定結果
    };

    std::list<NoteJudgeInfo> hitNotes;// 判定を拾ったノーツ一覧
    for(auto& note : nearNotes){
        // 正しい時間との差を取得
        float signedDif = note.lock()->time_ + PlaySettings::GetInstance()->GetOffsetJudge() - time;
        float dif = std::abs(signedDif);

        // 判定を拾ったノーツをリストにする
        Judgement::Evalution evaluation = note.lock()->Judge(dif);
        if(evaluation != Evalution::MISS){

            NoteJudgeInfo info;
            info.note = note;
            info.signedDif = signedDif;
            info.dif = dif;
            info.evaluation = evaluation;

            hitNotes.push_back(info);
        }
    }

    /*--------------------------*/
    // 重複判定の解決などを行う
    /*--------------------------*/
    uint32_t hitBits = 0;
    for(auto& note : hitNotes){

        Note_Base* notePtr = note.note.lock().get();

        // すでにビットが立っているなら、重複しているのでスルー
        if(hitBits & notePtr->laneBit_){
            continue;
        } else{
            if(notePtr->noteType_ != NoteType::Hold){
                notePtr->isEnd_ = true;// ノーツを終了させる
            }

            // ビットを立てる
            hitBits |= notePtr->laneBit_;
            pPlayField_->SetEvalution(notePtr->laneBit_, notePtr->layer_, judgeColor_[note.evaluation]);// レーンを押下状態にする

            // エフェクトを出す
            pPlayField_->EmitEffect(notePtr->laneBit_, notePtr->layer_, (int)note.evaluation);

            // コンボを加算
            RythmGameManager::GetInstance()->AddCombo();

            // 評価を追加
            RythmGameManager::GetInstance()->AddEvaluation(note.evaluation);

            // fast,lateの計算
            if(note.evaluation != Evalution::PERFECT){
                if(note.signedDif > 0.0f){
                    // 遅れすぎた場合
                    RythmGameManager::GetInstance()->AddLateCount();

                } else {
                    // 早すぎた場合
                    RythmGameManager::GetInstance()->AddFastCount();
                }
            }
        }
    }
}

// ホ－ルドの終点を判定する
void Judgement::JudgeHoldEnd(Note_Hold* note){

    // ホールドノーツの終点を判定する
    Evalution evaluation = note->JudgeHoldEnd();

    // コンボの管理
    if(evaluation != Evalution::MISS){
        RythmGameManager::GetInstance()->AddCombo();// コンボを加算
        RythmGameManager::GetInstance()->AddEvaluation(evaluation);// 評価を追加

        // perfect以外の場合はfastになる(押している時間が足りていないため)
        if(evaluation != Evalution::PERFECT){
            RythmGameManager::GetInstance()->AddFastCount();
        }

    } else{
        RythmGameManager::GetInstance()->BreakCombo();// コンボを切る
        RythmGameManager::GetInstance()->AddEvaluation(Evalution::MISS);// ミスを追加
    }

    // 終点の判定に応じてエフェクトとか出す
    pPlayField_->SetEvalution(note->laneBit_, note->layer_, judgeColor_[(int)evaluation]);// レーンを押下状態にする
}


////////////////////////////////////////////////////////
// 描画
////////////////////////////////////////////////////////
void Judgement::Draw(){
}
