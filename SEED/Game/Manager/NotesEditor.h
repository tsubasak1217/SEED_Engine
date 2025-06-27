#pragma once
#include <string>
#include <list>
#include <unordered_map>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>

struct TempoData{
    float bpm = 150;
    int timeSignature_denominator = 4;// 拍子の分母
    int timeSignature_numerator = 4;// 拍子の分子
    int barCount = 1;// 拍子の総数
    float time;
    float CalcDuration() const {
        // (一拍の秒数) * (四分音符の数) ←一般化式
        // (一拍の秒数) * (小節数) * (拍子の分母) * (4分に戻すための比率) ←少し展開した式
        return (60.0f / bpm) * barCount * timeSignature_denominator * (4.0f / timeSignature_numerator);
    }

    void Edit();
};

class NotesEditor{
public:
    NotesEditor();
    ~NotesEditor() = default;
    void EditNotes();

private:
    void DisplayLane();
    void DisplayTempoData();
    void EditTempoData();
private:
    float curLaneTime_ = 0.0;
    float timeScale_ = 1.0;
    float kVisibleTime_ = 2.0; // 可視範囲の時間

    ImVec2 laneLTPos_;
    ImVec2 laneSize_;
    ImVec2 tempoDataDisplayPos_;

    unordered_map<std::string, ImTextureID> textureIDs_;
    std::list<TempoData> tempoDataList_;

    TempoData* selectedTempoData_ = nullptr; // 選択中のテンポデータ
    TempoData* contextMenuTempoData_ = nullptr; // コンテキストメニュー用のテンポデータ
};