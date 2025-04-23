#include "NotesData.h"
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <Game/Objects/Judgement/PlayerInput.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <SEED/Lib/Structs/Range1D.h>

// ノーツのインクルード
#include <Game/Objects/Notes/Note_Hold.h>
#include <Game/Objects/Notes/Note_Tap.h>
#include <Game/Objects/Notes/Note_SideFlick.h>

////////////////////////////////////////////////////////////////////
// 時間から付近のノーツを取得する
////////////////////////////////////////////////////////////////////
std::vector<std::weak_ptr<Note_Base>> NotesData::GetNearNotes(float time){

    // 整列されたnotesからtimeに近いノーツを取得
    std::vector<std::weak_ptr<Note_Base>> nearNotes;

    // 判定範囲を取得
    float borderTime = Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD);
    Range1D range(time - borderTime, time + borderTime);

    // 範囲内にあるノーツを入れていく
    for(auto& note : notes_){
        if(MyFunc::IsContain(range, note.first)){
            nearNotes.push_back(note.second);
        }
    }

    return nearNotes;
}

////////////////////////////////////////////////////////////////////
// 条件を満たしたノーツを削除する
////////////////////////////////////////////////////////////////////
void NotesData::DeleteNotes(){

    // ボーダーとなる時間を計算
    float borderTime = currentTime_ + Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD);

    // 条件を満たすノーツを削除する
    for(auto it = notes_.begin(); it != notes_.end();){

        // ノーツが終わっているなら削除
        if(it->second->isEnd_){
            it->second.reset();
            it = notes_.erase(it);
            continue;
        }

        // 判定時間を超えていたら
        if(it->first < borderTime){

            // ホールドノーツの場合
            if(Note_Hold* holdNote = dynamic_cast<Note_Hold*>(it->second.get())){

                // 後ろがまだ判定ラインよりも前ならアクティブなホールドノーツに追加
                if(holdNote->time_ + holdNote->holdTime_ > borderTime){

                    // まだアクティブリストに入っていないなら追加
                    if(holdNote->isStackedToHoldList_ == false){
                        holdNote->isStackedToHoldList_ = true;
                        activeHoldNotes_.push_back(it->second);
                    }
                } else{
                    // 後ろが判定ラインよりも前なら削除
                    it->second.reset();
                    it = notes_.erase(it);
                }

            } else{
                // ホールドノーツでなければノーツを削除
                it->second.reset();
                it = notes_.erase(it);
            }
        } else{
            ++it;
        }
    }

    // 解放されたホールドノーツを削除
    for(auto it = activeHoldNotes_.begin(); it != activeHoldNotes_.end();){
        // ノーツが終わっているなら削除
        if(it->expired()){
            it = activeHoldNotes_.erase(it);
            continue;
        } else{
            ++it;
        }
    }
}

////////////////////////////////////////////////////////////////////
// ノーツデータのコンストラクタ
////////////////////////////////////////////////////////////////////
NotesData::NotesData(){
}

////////////////////////////////////////////////////////////////////
// ノーツデータのデストラクタ
////////////////////////////////////////////////////////////////////
NotesData::~NotesData(){
}

////////////////////////////////////////////////////////////////////
// ノーツデータの初期化
////////////////////////////////////////////////////////////////////
void NotesData::Initialize(){
    // 譜面の長さを初期化
    currentTime_ = 0.0f;
    // ノーツの初期化
    notes_.clear();
    activeHoldNotes_.clear();
}

////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////
void NotesData::Update(){

    // 時間を進める
    currentTime_ += ClockManager::DeltaTime();

    // 条件を満たすノーツの削除
    DeleteNotes();
}

////////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////////
void NotesData::Draw(){
}


