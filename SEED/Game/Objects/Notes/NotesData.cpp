#include "NotesData.h"
#include <SEED/Lib/Functions/MyFunc.h>
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
#include <Game/Objects/Notes/Note_Warning.h>

// 固有managerやエディタのインクルード
#include <Game/Manager/RythmGameManager.h>
#include <Game/Manager/NotesEditor.h>

////////////////////////////////////////////////////////////////////
// ノーツデータのコンストラクタ
////////////////////////////////////////////////////////////////////
NotesData::NotesData(){

    // タイマーの初期化
    waitTimer_.Initialize(6.0f);

    // 音源情報の初期化
    AudioManager::LoadAudio(AudioDictionary::Get("Metronome")); // メトロノームのロード
    AudioManager::LoadAudio(AudioDictionary::Get("Answer")); // アンサー音のロード
}


////////////////////////////////////////////////////////////////////
// ノーツデータのデストラクタ
////////////////////////////////////////////////////////////////////
NotesData::~NotesData(){
    AudioManager::EndAudio(songAudioHandle_); // 音源の終了
}

////////////////////////////////////////////////////////////////////
// ノーツデータの初期化
////////////////////////////////////////////////////////////////////
void NotesData::Initialize(const nlohmann::json& songData, const std::string& jsonPath){
    // 譜面の長さを初期化
    songTimer_.Reset();

    // ノーツの初期化
    notes_.clear();
    activeHoldNotes_.clear();
    onFieldNotes_.clear();

    // jsonからノーツデータを読み込む
    FromJson(songData);

    // 譜面データのファイルパスを保存
    jsonPath_ = jsonPath;
}

////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////
void NotesData::Update(){

    // ポーズ中は何もしない
    if(isPauseMode_){
        return;
    }


    // 待機時間が終了していたら譜面再生
    if(waitTimer_.IsFinished()){

        // 条件を満たすノーツの削除
        DeleteNotes();

        // 譜面の時間を進める
        songTimer_.Update();

        // 譜面が終了したらプレイ終了
        if(songTimer_.IsFinished()){
            isEnd_ = true;
        }

        // 音源の再生
        PlayAudio();

    } else{
        // 待機時間を進める
        waitTimer_.Update();

        // すこし経過したら「Ready?」テキストを出現させる
        if(waitTimer_.GetTrigger(1.0f)){
            auto* hierarchy = GameSystem::GetScene()->GetHierarchy();
            hierarchy->LoadObject2D("PlayScene/readyText.prefab");
        }
    }


    // 出現させるノーツの確認
    AppearNotes();
}

////////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////////
void NotesData::Draw(){

    // ノーツの描画(hold->tap->警告→wheel->rectFlickの順)
    // 奥から描画するためreverse_viewで逆順にする

    for(auto& note : std::ranges::reverse_view(onFieldNotes_)){
        if(auto notePtr = note.lock()){
            if(notePtr->noteType_ == NoteType::Hold){
                notePtr->Draw(songTimer_.currentTime, PlaySettings::GetInstance()->GetLaneNoteAppearTime());
            }
        }
    }

    for(auto& note : std::ranges::reverse_view(onFieldNotes_)){
        if(auto notePtr = note.lock()){
            if(notePtr->noteType_ == NoteType::Tap){
                notePtr->Draw(songTimer_.currentTime, PlaySettings::GetInstance()->GetLaneNoteAppearTime());
            }
        }
    }

    for(auto& note : std::ranges::reverse_view(onFieldNotes_)){
        if(auto notePtr = note.lock()){
            if(notePtr->noteType_ == NoteType::Warning){
                notePtr->Draw(songTimer_.currentTime, PlaySettings::GetInstance()->GetLaneNoteAppearTime());
            }
        }
    }

    for(auto& note : std::ranges::reverse_view(onFieldNotes_)){
        if(auto notePtr = note.lock()){
            if(notePtr->noteType_ == NoteType::Wheel){
                notePtr->Draw(songTimer_.currentTime, PlaySettings::GetInstance()->GetLaneNoteAppearTime());
            }
        }
    }

    for(auto& note : std::ranges::reverse_view(onFieldNotes_)){
        if(auto notePtr = note.lock()){
            if(notePtr->noteType_ == NoteType::RectFlick){
                notePtr->Draw(songTimer_.currentTime, PlaySettings::GetInstance()->GetKNoteAppearTime());
            }
        }
    }

}

////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
////////////////////////////////////////////////////////////////////
void NotesData::BeginFrame(){
    if(isPauseMode_){
        return;
    }

    onFieldNotes_.clear();// フィールド上のノーツをクリア
}


////////////////////////////////////////////////////////////////////
// 時間から付近のノーツを取得する
////////////////////////////////////////////////////////////////////
std::vector<std::weak_ptr<Note_Base>> NotesData::GetNearNotes(float time){

    // 整列されたnotesからtimeに近いノーツを取得
    std::vector<std::weak_ptr<Note_Base>> nearNotes;

    // 判定範囲を取得
    float borderTime = Judgement::GetInstance()->GetJudgeTime(Judgement::Evalution::GOOD);
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
// 最大コンボ数を取得
////////////////////////////////////////////////////////////////////
int32_t NotesData::GetTotalCombo(){
    int32_t totalCombo = 0;
    for(auto& note : notes_){
        if(Note_Hold* holdNote = dynamic_cast<Note_Hold*>(note.second.get())){
            totalCombo += 2;
        } else{
            if(note.second->noteType_ != NoteType::Warning){
                totalCombo++;
            }
        }
    }

    return totalCombo;
}

////////////////////////////////////////////////////////////////////
// 条件を満たしたノーツを削除する
////////////////////////////////////////////////////////////////////
void NotesData::DeleteNotes(){

    // ボーダーとなる時間を計算
    float borderTime = songTimer_.currentTime - Judgement::GetInstance()->GetJudgeTime(Judgement::Evalution::GOOD);

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
                if(holdNote->time_ + holdNote->kHoldTime_ >= songTimer_.currentTime){

                    // まだアクティブリストに入っていないなら追加
                    if(holdNote->isStackedToHoldList_ == false){
                        holdNote->isStackedToHoldList_ = true;
                        activeHoldNotes_.push_back(it->second);

                        // 先頭を押せなかった場合
                        if(holdNote->headEvaluation_ == Judgement::Evalution::NONE){
                            RythmGameManager::GetInstance()->BreakCombo(); // コンボを切る
                            RythmGameManager::GetInstance()->AddEvaluation(Judgement::Evalution::MISS);// ミスを追加

                        }
                    }

                    ++it;
                } else{
                    // 後ろが判定ラインよりも前なら終点の判定を行い削除
                    Judgement::GetInstance()->JudgeHoldEnd(holdNote);
                    it->second.reset();
                    it = notes_.erase(it);
                }

            } else{
                if(it->second->noteType_ != NoteType::Warning){
                    // ホールド,警告ノーツでなければノーツを削除
                    it->second.reset();
                    it = notes_.erase(it);
                    // 押せずに通り過ぎたノーツなのでコンボを切る
                    RythmGameManager::GetInstance()->BreakCombo(); // コンボを切る
                    RythmGameManager::GetInstance()->AddEvaluation(Judgement::Evalution::MISS);// ミスを追加

                } else{
                    if(Note_Warning* warningNote = dynamic_cast<Note_Warning*>(it->second.get())){
                        warningNote->start_ = true;
                        // 警告ノーツの場合、表示時間を過ぎていたら削除
                        if(warningNote->timer_.IsFinished()){
                            it->second.reset();
                            it = notes_.erase(it);
                        } else{
                            ++it;
                        }
                    } else{
                        ++it;
                    }

                }
            }
        } else{
            ++it;
        }
    }

    float judgeTime = GetCurMusicTime() + PlaySettings::GetInstance()->GetOffsetJudge();

    // 解放されたホールドノーツを削除
    for(auto it = activeHoldNotes_.begin(); it != activeHoldNotes_.end();){
        // ノーツが終わっているなら削除
        if(it->expired()){
            it = activeHoldNotes_.erase(it);
            continue;
        } else{
            it->lock()->Judge(judgeTime);
            ++it;
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// ノーツを出現させる
//////////////////////////////////////////////////////////////////////
void NotesData::AppearNotes(){
    // ノーツの時間とレーンスピードから出現させるかどうか決める
    float laneNotesBorderTime = songTimer_.currentTime + PlaySettings::GetInstance()->GetLaneNoteAppearTime();

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

//////////////////////////////////////////////////////////////////////
// 音源のの再生処理
//////////////////////////////////////////////////////////////////////
void NotesData::PlayAudio(){

    debugTimer_.Update();

    if(songTimer_.currentTime >= startOffsetTime_){
        // 再生されていないなら再生
        if(!isSongStarted_){
            AudioManager::EndAudio(songAudioHandle_); // 前の音源を終了
            songAudioHandle_ = AudioManager::PlayAudio(songFilePath_, false, 1.0f, songTimer_.currentTime - startOffsetTime_);
            isSongStarted_ = true;

        } else{
            if(!isEnd_){
                // 音ズレの許容時間と比較してズレていたら補正
                static float toleranceTime = 1.0f / 60.0f;
                float audioTime = AudioManager::GetAudioPlayTime(songAudioHandle_);

                // タイマーを音源に合わせる
                float dif = (songTimer_.currentTime - startOffsetTime_) - audioTime;
                if(std::fabsf(dif) > toleranceTime){
                    if(audioTime > 0.5f){
                        debugTimer_.Reset();
                        songTimer_.currentTime -= dif;
                    }
                }
            }
        }

    } else{
        // 再生されているなら停止
        isSongStarted_ = false;
        if(AudioManager::IsPlayingAudio(songAudioHandle_)){
            AudioManager::EndAudio(songAudioHandle_);
        }

        // メトロノームの再生
        static float signatureTime = 0.0f;
        signatureTime = tempoDataList_.front().GetBeatDuration();

        // 拍のラインを超えた瞬間を検出
        int signatureCount[2] = {
            (int)std::ceil(songTimer_.currentTime / signatureTime),
            (int)std::ceil(songTimer_.prevTime / signatureTime)
        };

        // 拍のラインを超えた瞬間であればメトロノームを鳴らす
        if((signatureCount[0] != signatureCount[1])){
            // メトロノームの再生
            AudioManager::PlayAudio(AudioDictionary::Get("Metronome"), false);
        }
    }


    // アンサー音の再生
    // ボーダーとなる時間を計算
    float borderTime[2] = {
        songTimer_.currentTime - PlaySettings::GetInstance()->GetTotalOffsetAnswerSE(),
        songTimer_.prevTime - PlaySettings::GetInstance()->GetTotalOffsetAnswerSE()
    };

    // アンサー音を鳴らすべきか判定
    for(auto it = answerTimes_.begin(); it != answerTimes_.end();){

        // 今の時間より後ろなら終了
        if(*it > borderTime[0]){
            break;
        }

        // ボーダー時間内を超えたばかりならアンサー音を鳴らす
        if(*it <= borderTime[0] && *it > borderTime[1]){
            // 鳴らしたら削除
            AudioManager::PlayAudio(AudioDictionary::Get("Answer"), false);
            it = answerTimes_.erase(it);

        } else{
            ++it;
        }
    }

}

//////////////////////////////////////////////////////////////////////
// 音源の一時停止,再開
//////////////////////////////////////////////////////////////////////
void NotesData::Pause(){
    isPauseMode_ = true;
    isStopped_ = true;
    songTimer_.Stop();
    AudioManager::EndAudio(songAudioHandle_);
}

void NotesData::Resume(){
    isPauseMode_ = false;
    isStopped_ = false;
    // 再生状態にする
    songTimer_.Restart();
    if(songTimer_.currentTime >= startOffsetTime_){
        songAudioHandle_ = AudioManager::PlayAudio(songFilePath_, false, 1.0f, songTimer_.currentTime - startOffsetTime_);
    }
}

//////////////////////////////////////////////////////////////////////
// 指定した時間範囲に指定した形式のノーツが存在するか検索する
//////////////////////////////////////////////////////////////////////
bool NotesData::SearchNoteByTime(float minTime, float maxTime, NoteType noteType)const{
    Range1D searchRange(minTime, maxTime);
    for(auto& note : notes_){
        if(MyFunc::IsContain(searchRange, note.first)){
            if(note.second->noteType_ == noteType){
                return true;
            }
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////
// JSONからノーツデータを読み込む
//////////////////////////////////////////////////////////////////////
void NotesData::FromJson(const nlohmann::json& songData){

    // JSONデータを保存
    songData_ = songData;

    // 曲のファイルパスを取得
    if(songData.contains("audioPath")){
        songFilePath_ = songData["audioPath"];
        AudioManager::LoadAudio(songFilePath_); // オーディオをロード
    }

    // 曲全体のオフセット時間を取得
    if(songData.contains("offsetTime")){
        songOffsetTime_ = songData["offsetTime"];
    }

    // 読み込みオフセット = 譜面自体のオフセット + プレイヤー設定のオフセット
    float loadOffset = songOffsetTime_ + PlaySettings::GetInstance()->GetOffsetView();

    // ノーツデータの読み込み
    if(songData.contains("notes")){
        for(const auto& noteJson : songData["notes"]){
            std::string noteType = noteJson["noteType"];

            if(noteType == "tap"){
                Note_Tap* note = new Note_Tap();
                note->FromJson(noteJson);
                note->time_ += loadOffset;
                notes_.emplace_back(std::make_pair(note->time_, note));

            } else if(noteType == "hold" or noteType == "Hold"){
                Note_Hold* note = new Note_Hold();
                note->FromJson(noteJson);
                note->time_ += loadOffset;
                notes_.emplace_back(std::make_pair(note->time_, note));

            } else if(noteType == "rectFlick" or noteType == "RectFlick"){
                Note_RectFlick* note = new Note_RectFlick();
                note->FromJson(noteJson);
                note->time_ += loadOffset;
                notes_.emplace_back(std::make_pair(note->time_, note));

            } else if(noteType == "wheel"){
                Note_Wheel* note = new Note_Wheel();
                note->FromJson(noteJson);
                note->time_ += loadOffset;
                notes_.emplace_back(std::make_pair(note->time_, note));

            } else if(noteType == "warning"){
                Note_Warning* note = new Note_Warning();
                note->FromJson(noteJson);
                note->time_ += loadOffset;
                notes_.emplace_back(std::make_pair(note->time_, note));

            } else{
                // 未知のノーツタイプは無視
                continue;
            }

            // 左右反転の設定が有効なら、ノーツのレーンを反転
            if(PlaySettings::GetInstance()->GetIsReverseLR()){
                notes_.back().second->lane_ -= 2;
                notes_.back().second->lane_ *= -1; // レーンを反転
                notes_.back().second->lane_ += 2;
            }

            // 上下反転の設定が有効なら、ノーツのレイヤーを反転
            if(PlaySettings::GetInstance()->GetIsReverseUD()){
                UpDown layer = notes_.back().second->layer_;
                notes_.back().second->layer_ = layer == UpDown::UP ? UpDown::DOWN : UpDown::UP; // レイヤーを反転
            }
        }
    }

    // テンポデータの読み込み
    if(songData.contains("tempoData")){
        for(const auto& tempoJson : songData["tempoData"]){
            TempoData tempoData;
            tempoData.FromJson(tempoJson);
            tempoDataList_.push_back(tempoData);
        }
    }

    // ノーツを時間でソート
    std::sort(notes_.begin(), notes_.end(),
        [](const std::pair<float, std::shared_ptr<Note_Base>>& a,
            const std::pair<float, std::shared_ptr<Note_Base>>& b){
        return a.first < b.first;
    });


    // 時間を計算
    if(tempoDataList_.size() != 0){
        startOffsetTime_ = tempoDataList_.front().CalcDuration();
        float duration = tempoDataList_.back().time + tempoDataList_.back().CalcDuration();
        songTimer_.duration = duration; // 譜面の長さを設定
    }

    // アンサー音を鳴らす時間のリストを作成
    answerTimes_.clear();
    for(auto& note : notes_){
        // 警告ノーツはアンサー音を鳴らさないため無視
        if(Note_Warning* warningNote = dynamic_cast<Note_Warning*>(note.second.get())){
            continue;
        }

        // ノーツの開始時間を追加
        answerTimes_.push_back(note.first);

        // ホールドノーツは終点も鳴らすため追加
        if(Note_Hold* holdNote = dynamic_cast<Note_Hold*>(note.second.get())){
            answerTimes_.push_back(holdNote->time_ + holdNote->kHoldTime_);
        }
    }

    // 時間でソート
    answerTimes_.sort();
}

//////////////////////////////////////////////////////////////////////
// 譜面データのホットリロードを行う関数
//////////////////////////////////////////////////////////////////////
void NotesData::HotReload(){
    // 譜面データがなければアサート
    if(songData_.empty()){
        // jsonPathからもう一度読み込む
        songData_ = nlohmann::json::parse(std::ifstream(jsonPath_));
    }

    // JSONファイルを読み込む
    std::string jsonFilePath = songData_["jsonFilePath"];
    songData_ = nlohmann::json::parse(std::ifstream(jsonFilePath));

    // ノーツの初期化
    notes_.clear();
    activeHoldNotes_.clear();
    onFieldNotes_.clear();

    // jsonからノーツデータを読み込む
    FromJson(songData_);
}

//////////////////////////////////////////////////////////////////////
// ImGuiから触る用の関数
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void NotesData::Edit(){

    static ImTextureID playIcon = nullptr;
    static ImTextureID pauseIcon = nullptr;

    if(!playIcon or !pauseIcon){
        playIcon = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/play2.png");
        pauseIcon = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/play.png");
    }

    ImFunc::CustomBegin("NotesData", MoveOnly_TitleBar);
    {

        // 再生・停止・時間変更
        if(ImGui::ImageButton("playIcon", isStopped_ ? pauseIcon : playIcon, ImVec2(20, 20))){
            if(!isPauseMode_){
                isStopped_ = !isStopped_;

                if(isStopped_){
                    Pause();
                } else{
                    Resume();
                }
            }
        }

        ImGui::SameLine();
        if(ImGui::SliderFloat("曲時間", &songTimer_.currentTime, 0.0f, songTimer_.duration)){
            AudioManager::EndAudio(songAudioHandle_);

            // ノーツの初期化
            notes_.clear();
            activeHoldNotes_.clear();
            onFieldNotes_.clear();

            // jsonからノーツデータを読み込む
            FromJson(songData_);

        } else{
            if(!isStopped_ && !isEnd_){
                if(songTimer_.currentTime >= startOffsetTime_){
                    if(!AudioManager::IsPlayingAudio(songAudioHandle_)){
                        songAudioHandle_ =
                            AudioManager::PlayAudio(
                                songFilePath_, false, 1.0f, songTimer_.currentTime - startOffsetTime_
                            );
                    }
                }
            }
        }

        // 音ズレの表示
        if(isSongStarted_){
            float dif = std::fabsf((songTimer_.currentTime - startOffsetTime_) - AudioManager::GetAudioPlayTime(songAudioHandle_));
            ImGui::Text("音ズレ: %.4f 秒", dif);
        }

        if(debugTimer_.GetProgress() != 1.0f){
            ImGui::Text("音ズレ修正した");
        }

        // ホットリロードのボタン
        if(ImGui::Button("譜面をホットリロード")){
            HotReload();
        }


        ImGui::End();
    }


    // 
    if(Input::IsTriggerKey(DIK_RIGHT)){

    }
}
#endif // _DEBUG
