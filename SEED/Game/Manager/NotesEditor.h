#pragma once
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <Game/Objects/Notes/Note_Base.h>

struct TempoData{
    float bpm = 150;
    int timeSignature_denominator = 4;// 拍子の分母
    int timeSignature_numerator = 4;// 拍子の分子
    int barCount = 1;// 拍子の総数
    float time;
    float CalcDuration() const {
        // (一拍の秒数) * (四分音符の数) ←一般化式
        // (一拍の秒数) * (小節数) * (拍子の分子) * (4分に戻すための比率) ←少し展開した式
        return (60.0f / bpm) * barCount * timeSignature_numerator * (4.0f / timeSignature_denominator);
    }

    void Edit();
};

class NotesEditor{
private:
    enum EditNoteType{
        Tap,
        Hold,
        RectFlick,
        Wheel
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
        float beatTime; // 1拍の時間(秒)
        float beatYHeight;
        TempoData* parent = nullptr;
    };

public:
    NotesEditor();
    ~NotesEditor() = default;
    void Edit();

private:
    void UpdateTimeScale();
    void DisplayLane();
    void DisplayLine();
    void DisplayTempoData();
    void EditTempoData();
    void EditNotes();
    void CreateNoteOnLane();
    void DisplayNotes();
    void DraggingNote();

private:
    // 基礎情報
    float duration_ = 0.0f; // 曲の長さ
    float curLaneTime_ = 0.0;
    float timeScale_ = 1.0;
    float kVisibleTime_ = 2.0; // 可視範囲の時間
    bool isPlaying_ = false; // 再生中かどうか
    bool isEditOnLane_ = false; // レーン上での編集モードかどうか
    float division_ = 4;
    ImVec2 laneLTPos_;
    ImVec2 laneSize_;
    ImVec2 worldLaneLTPos_;
    ImVec2 tempoDataDisplayPos_;

    // 表示する情報
    unordered_map<std::string, ImTextureID> textureIDs_;
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
};