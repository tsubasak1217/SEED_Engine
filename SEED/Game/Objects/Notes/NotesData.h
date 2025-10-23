#pragma once
#include <vector>
#include <queue>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Game/Objects/Notes/Note_Base.h>
#include <SEED/Lib/Functions/MyFunc.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Structs/Timer.h>
struct TempoData;

/// <summary>
/// 譜面情報を保持するクラス
/// </summary>
class NotesData{
public:
    NotesData();
    ~NotesData();
    void Initialize(const nlohmann::json& songData);
    void Update();
    void Draw();
    void BeginFrame();

public:// アクセッサ
    float GetCurMusicTime(){ return songTimer_.currentTime; }
    std::vector<std::weak_ptr<Note_Base>> GetNearNotes(float time);
    float GetDuration(){ return songTimer_.duration; }
    bool GetIsEnd(){ return isEnd_; }
    int32_t GetTotalCombo();
    void AddActiveHoldNote(std::weak_ptr<Note_Base> note){activeHoldNotes_.push_back(note);}
    void Pause();
    void Resume();

private:// 内部関数
    void DeleteNotes();
    void AppearNotes();// ノーツを出現させる
    void PlayAudio();

private:// 入出力
    void FromJson(const nlohmann::json& songData);
    void HotReload();

private:
    // 譜面データ(ファイル)
    nlohmann::json songData_;// 譜面データ

    // タイマー関連
    Timer songTimer_;// 曲の再生時間
    Timer waitTimer_;// ノーツを出現させるまでの時間
    float startOffsetTime_ = 0.0f;// 曲の開始オフセット時間(メトロノーム流す時間)
    bool isStopped_ = false;// 曲が停止しているかどうか
    bool isPauseMode_ = false;// 一時停止モードかどうか
    bool isEnd_ = false;// 曲が終了したかどうか

    // 音源関連
    std::string songFilePath_;// 曲のファイルパス
    std::string metronomeFilePath_;// メトロノームのファイルパス
    std::string answerSEFilePath_;// 正解SEのファイルパス
    AudioHandle songAudioHandle_;// 曲の音源ハンドル
    bool isSongStarted_ = false;// 曲が開始されたかどうか

    // 譜面データ関連
    std::vector<std::pair<float, std::shared_ptr<Note_Base>>> notes_;// すべてのノーツ
    std::vector<std::weak_ptr<Note_Base>> activeHoldNotes_;// アクティブなホールドノーツ
    std::list<std::weak_ptr<Note_Base>> onFieldNotes_;// フィールド上のノーツ(見えているノーツ)
    std::list<TempoData> tempoDataList_;

public:
#ifdef _DEBUG
    void Edit();
#endif // _DEBUG
};