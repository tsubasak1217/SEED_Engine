#pragma once
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <json.hpp>
#include <SEED/Source/SEED.h>
#include <Game/Objects/SongSelect/SongInfo.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Functions/MyFunc.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <Game/Objects/Notes/Note_Base.h>

/// <summary>
/// 譜面のテンポ情報を持つクラス
/// </summary>
struct TempoData{
    float bpm = 150;
    int timeSignature_denominator = 4;// 拍子の分母
    int timeSignature_numerator = 4;// 拍子の分子
    int barCount = 1;// 拍子の総数
    float time;
    bool removeFlag = false;

    float GetBeatDuration() const {
        // 1拍の時間(秒) = 60 / BPM
        return 60.0f / bpm;
    }

    float CalcDuration() const {
        // (一拍の秒数) * (四分音符の数) ←一般化式
        // (一拍の秒数) * (小節数) * (拍子の分子) * (4分に戻すための比率) ←少し展開した式
        return (60.0f / bpm) * barCount * timeSignature_numerator * (4.0f / timeSignature_denominator);
    }

    void Edit();
    nlohmann::json ToJson() const {
        nlohmann::json jsonData;
        jsonData["bpm"] = bpm;
        jsonData["timeSignature_denominator"] = timeSignature_denominator;
        jsonData["timeSignature_numerator"] = timeSignature_numerator;
        jsonData["barCount"] = barCount;
        jsonData["time"] = time;
        return jsonData;
    }

    void FromJson(const nlohmann::json& jsonData) {
        bpm = jsonData["bpm"];
        timeSignature_denominator = jsonData["timeSignature_denominator"];
        timeSignature_numerator = jsonData["timeSignature_numerator"];
        barCount = jsonData["barCount"];
        time = jsonData["time"];
    }
};

class NotesEditor{
private:
    enum EditNoteType{
        Tap,
        Hold,
        RectFlick,
        Wheel,
        Warning,
    };

    enum DivisionMode{
        Div_4,
        Div_6,
        Div_8,
        Div_12,
        Div_16,
        Div_24,
        Div_32,
        Div_48,
        Div_64,
    };

    struct DivisionData{
        float laneYPosition;
        float timeOnLane;
        float beatDuration; // 1拍の時間(秒)
        float beatYHeight;
        float timeOfStartSignature;
        float heightOfStartSignature;
        bool isStartOfSignature = false;
        TempoData* parent = nullptr;
    };

public:
    NotesEditor();
    ~NotesEditor() = default;
    void Initialize(const std::string& path);
    void Edit();

private:
    void SelectAudioFile();
    void UpdateTimeScale();
    void DisplayLane();
    void DisplayLine();
    void DisplayTempoData();
    void EditTempoData();
    void EditNotes();
    void CreateNoteOnLane();
    void DisplayNotes();
    void DraggingNote();
    void ScrollOnLane();
    void EditSelectNote();
    void EraseCheck();
    void PlayMetronome();
    void PlayAnswerSE();
    void FileControl();

private:
    // 基礎情報
    int32_t difficulty_ = 0; // 難易度
    float duration_ = 0.0f; // 曲の長さ
    float curLaneTime_ = 0.0;
    float timeScale_ = 1.0;
    float kVisibleTime_ = 2.0; // 可視範囲の時間
    bool isPlaying_ = false; // 再生中かどうか
    bool isEditOnLane_ = false; // レーン上での編集モードかどうか
    float division_ = 4;
    bool isHoveringNote_ = false; // ノート上にカーソルがあるかどうか
    float answerOffsetTime_;
    float metronomeOffsetTime_; // メトロノームのオフセット時間
    ImVec2 laneLTPos_;
    ImVec2 laneSize_;
    ImVec2 worldLaneLTPos_;
    ImVec2 tempoDataDisplayPos_;
    std::string artistName_; // アーティスト名
    std::string notesDesignerName_; // ノーツ制作者名
    SongGenre songGenre_; // 曲のジャンル
    float offsetTime_ = 0.0f; // 曲のオフセット時間

    // 表示する情報
    unordered_map<std::string, ImTextureID> textureIDs_;
    unordered_map<LaneBit, std::string> laneTextureNameMap_;
    std::list<TempoData> tempoDataList_;

    // ポインタ
    TempoData* selectedTempoData_ = nullptr; // 選択中のテンポデータ
    TempoData* contextMenuTempoData_ = nullptr; // コンテキストメニュー用のテンポデータ
    ImDrawList* pDrawList_ = nullptr;
    Note_Base* draggingNote_ = nullptr; // ドラッグ中のノート
    Note_Base* edittingNote_ = nullptr; // 編集中のノート

    // 編集モードなどの情報
    EditNoteType editNoteType_ = Tap; // 編集中のノートタイプ
    DivisionMode divisionMode_ = Div_4; // 分割モード(配置の際の分割数)

    // ノーツ配置に利用する境界線情報
    std::vector<DivisionData> upLayerBorders_;
    std::vector<DivisionData> downLayerBorders_;
    std::vector<float> laneBorders_;

    // 配置されたノートのリスト
    std::list<std::unique_ptr<Note_Base>> notes_;

    // 音声情報
    std::string audioFilePath_;
    AudioHandle audioHandle_;

    // 入出力ファイル名
    std::string saveDifficultyName_ = "Basic";
    std::string saveSongName_;
    std::string loadFileName_;
    std::string jsonFilePath_;

    void LoadFromJson(const nlohmann::json& jsonData);
    nlohmann::json ToJson() ;
};