#pragma once
#include <vector>
#include <queue>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Game/Objects/Notes/Note_Base.h>
#include <SEED/Lib/Functions/MyFunc.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Structs/Timer.h>
#include <Game/Objects/SongSelect/SongInfo.h>

struct TempoData;

/// <summary>
/// 譜面情報を保持するクラス
/// </summary>
class NotesData{

    struct AnswerInfo{
        float time;
        bool isExNote;
    };

public:
    NotesData();
    ~NotesData();
    void Initialize(const SongInfo& songInfo, int32_t difficulty);
    void Update();
    void Draw();
    void BeginFrame();

public:// アクセッサ
    float GetCurMusicTime()const{ return songTimer_.currentTime; }
    std::vector<std::weak_ptr<Note_Base>> GetNearNotes(float time);
    float GetDuration(){ return songTimer_.duration; }
    const Timer* GetSongTimerPtr(){ return &songTimer_; }
    bool GetIsEnd(){ return isEnd_; }
    int32_t GetTotalCombo();
    void AddActiveHoldNote(std::weak_ptr<Note_Base> note){activeHoldNotes_.push_back(note);}
    void Pause();
    void Resume();
    // 指定した時間範囲に指定した形式のノーツが存在するか検索する
    bool SearchNoteByTime(float minTime, float maxTime, NoteType noteType)const;

private:// 内部関数
    void DeleteNotes();
    void AppearNotes();// ノーツを出現させる
    void PlayAudio();

private:// 入出力
    void FromJson(const SongInfo& songInfo, int32_t difficulty);
    void HotReload();

private:
    // 譜面データ(ファイル)
    SongInfo songInfo_;// 楽曲データ
    int32_t playDifficulty_ = 0;// 選択された難易度
    std::string jsonPath_;// 譜面データのファイルパス

    // タイマー関連
    Timer songTimer_;// 曲の再生時間
    Timer waitTimer_;// ノーツを出現させるまでの時間
    float startOffsetTime_ = 0.0f;// 曲の開始オフセット時間(メトロノーム流す時間)
    bool isStopped_ = false;// 曲が停止しているかどうか
    bool isPauseMode_ = false;// 一時停止モードかどうか
    bool isEnd_ = false;// 曲が終了したかどうか

    // 音源関連
    std::string songFilePath_;// 曲のファイルパス
    AudioHandle songAudioHandle_;// 曲の音源ハンドル
    bool isSongStarted_ = false;// 曲が開始されたかどうか
    float audioVolume_ = 1.0f;// 曲の音量

    // 譜面データ関連
    std::vector<std::pair<float, std::shared_ptr<Note_Base>>> notes_;// すべてのノーツ
    std::list<AnswerInfo> answerInfo_;// 判定音を鳴らす情報のリスト
    std::vector<std::weak_ptr<Note_Base>> activeHoldNotes_;// アクティブなホールドノーツ
    std::list<std::weak_ptr<Note_Base>> onFieldNotes_;// フィールド上のノーツ(見えているノーツ)
    std::list<TempoData> tempoDataList_;
    float songOffsetTime_ = 0.0f;// 曲全体のオフセット時間

    Timer debugTimer_ = Timer(2.0f,2.0f);

public:
#ifdef _DEBUG
    void Edit();
#endif // _DEBUG
};