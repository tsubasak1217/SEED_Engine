#include "NotesData.h"
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <Game/Objects/Judgement/PlayerInput.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <Game/Config/PlaySettings.h>
#include <SEED/Lib/Structs/Range1D.h>

// ノーツのインクルード
#include <Game/Objects/Notes/Note_Hold.h>
#include <Game/Objects/Notes/Note_Tap.h>
#include <Game/Objects/Notes/Note_SideFlick.h>
#include <Game/Objects/Notes/Note_RectFlick.h>
#include <Game/Objects/Notes/Note_Wheel.h>

////////////////////////////////////////////////////////////////////
// ノーツデータのコンストラクタ
////////////////////////////////////////////////////////////////////
NotesData::NotesData(){
}

// テスト用にランダムノーツを生成するコンストラクタ
NotesData::NotesData(bool isRandomNotes){
    // 譜面の長さを初期化
    duration_ = 64.0f;
    currentTime_ = 0.0f;

    // ノーツの初期化
    notes_.clear();
    activeHoldNotes_.clear();

    // ランダムノーツを生成する(いったんタップのみ)
    int numNotes = 1000;
    if(isRandomNotes){
        for(int i = 0; i < numNotes; i++){

            {// Tapノーツ確認用
                //float time = 10.0f + 0.1f * i;
                //int32_t lane = i % PlayField::kKeyCount_;
                //UpDown layer = (UpDown)(rand() % 1);
                //std::shared_ptr<Note_Base> note = std::make_shared<Note_Tap>();
                //note->time_ = time;
                //note->lane_ = lane;
                //note->layer_ = layer;
                //note->laneBit_ = (LaneBit)(1 << lane);
                //notes_.emplace_back(std::make_pair(time, note));
            }

            {// Wheelノーツ確認用
                float time = 10.0f + 1.0f * i;
                UpDown layer = UpDown(i % 2);
                std::shared_ptr<Note_Wheel> note = std::make_shared<Note_Wheel>();
                note->time_ = time;
                note->layer_ = layer;

                if(i % 2 == 0){
                    note->direction_ = UpDown::UP;
                    note->laneBit_ = LaneBit::WHEEL_UP;
                } else{
                    note->direction_ = UpDown::DOWN;
                    note->laneBit_ = LaneBit::WHEEL_DOWN;
                }

                notes_.emplace_back(std::make_pair(time, note));
            }

            {// RectFlickノーツ確認用
                //float time = 5.0f + 0.5f * i;
                //int32_t dir = i % 13;
                //std::shared_ptr<Note_Base> note = std::make_shared<Note_RectFlick>();
                //note->time_ = time;
                //if(dir == 0){
                //    note->laneBit_ = LaneBit::RECTFLICK_LT;
                //} else if(dir == 1){
                //    note->laneBit_ = LaneBit::RECTFLICK_RT;
                //} else if(dir == 2){
                //    note->laneBit_ = LaneBit::RECTFLICK_RB;
                //} else if(dir == 3){
                //    note->laneBit_ = LaneBit::RECTFLICK_LB;
                //} else if(dir == 4){
                //    note->laneBit_ = LaneBit::RECTFLICK_UP;
                //} else if(dir == 5){
                //    note->laneBit_ = LaneBit::RECTFLICK_RIGHT;
                //} else if(dir == 6){
                //    note->laneBit_ = LaneBit::RECTFLICK_DOWN;
                //} else if(dir == 7){
                //    note->laneBit_ = LaneBit::RECTFLICK_LEFT;
                //} else if(dir == 8){
                //    note->laneBit_ = LaneBit::RECTFLICK_LT_EX;
                //} else if(dir == 9){
                //    note->laneBit_ = LaneBit::RECTFLICK_RT_EX;
                //} else if(dir == 10){
                //    note->laneBit_ = LaneBit::RECTFLICK_RB_EX;
                //} else if(dir == 11){
                //    note->laneBit_ = LaneBit::RECTFLICK_LB_EX;
                //} else{
                //    note->laneBit_ = LaneBit::RECTFLICK_ALL;
                //}

                //notes_.emplace_back(std::make_pair(time, note));
            }

            {// Holdノーツ確認用
                //float time = 5.0f + 3.0f * i;
                //int32_t lane = i % PlayField::kKeyCount_;
                //UpDown layer = UpDown::UP;
                //std::shared_ptr<Note_Hold> note = std::make_shared<Note_Hold>();
                //note->time_ = time;
                //note->kHoldTime_ = MyFunc::Random(1.0f, 3.0f);
                //note->lane_ = lane;
                //note->layer_ = layer;
                //note->laneBit_ = (LaneBit)(1 << lane);
                //notes_.emplace_back(std::make_pair(time, note));
            }
        }

        // ノーツを時間でソート
        std::sort(notes_.begin(), notes_.end(),
            [](const std::pair<float, std::shared_ptr<Note_Base>>& a,
                const std::pair<float, std::shared_ptr<Note_Base>>& b){
            return a.first < b.first;
        });
    }
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

    // 出現させるノーツの確認
    AppearNotes();
}

////////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////////
void NotesData::Draw(){
    // ノーツの描画
    for(auto& note : onFieldNotes_){
        if(auto notePtr = note.lock()){
            notePtr->Draw(currentTime_, PlaySettings::GetInstance()->GetLaneNoteAppearTime());
        }
    }
}

////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
////////////////////////////////////////////////////////////////////
void NotesData::BeginFrame(){
    onFieldNotes_.clear();// フィールド上のノーツをクリア
}


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
    float borderTime = currentTime_ - Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD);

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
                if(holdNote->time_ + holdNote->kHoldTime_ >= borderTime){

                    // まだアクティブリストに入っていないなら追加
                    if(holdNote->isStackedToHoldList_ == false){
                        holdNote->isStackedToHoldList_ = true;
                        activeHoldNotes_.push_back(it->second);
                    }

                    ++it;
                } else{
                    // 後ろが判定ラインよりも前なら終点の判定を行い削除
                    Judgement::GetInstance()->JudgeHoldEnd(holdNote);
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
            it->lock()->Judge(0.0f);
            ++it;
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// ノーツを出現させる
//////////////////////////////////////////////////////////////////////
void NotesData::AppearNotes(){
    // ノーツの時間とレーンスピードから出現させるかどうか決める
    float laneNotesBorderTime = currentTime_ + PlaySettings::GetInstance()->GetLaneNoteAppearTime();
    //float outsideNotesBorderTime = currentTime_ + PlaySettings::GetInstance()->GetOutsideNoteAppearTime();

    // borderTimeになるまでのノーツを出現させる
    for(int i = 0; i < notes_.size(); i++){
        // ノーツの時間を取得
        float noteTime = notes_[i].first;

        // ノーツの時間がborderTimeよりも前なら出現させる
        if(noteTime <= laneNotesBorderTime){
            // 出現ノーツに追加
            onFieldNotes_.push_back(notes_[i].second);
        } else{
            break;
        }
    }
}

