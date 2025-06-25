#pragma once
#include <vector>
#include <queue>
#include <Game/Objects/Notes/Note_Base.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>

class NotesData{
public:
    NotesData();
    NotesData(bool isRandomNotes);
    ~NotesData();
    void Initialize();
    void Update();
    void Draw();
    void BeginFrame();

public:
    float GetCurMusicTime(){ return currentTime_; }
    void AddNote();
    void RemoveNote();
    void LoadFromFile(const std::string& fileName);
    std::vector<std::weak_ptr<Note_Base>> GetNearNotes(float time);
    float GetDuration(){ return duration_; }
    void AddActiveHoldNote(std::weak_ptr<Note_Base> note){activeHoldNotes_.push_back(note);}

private:
    void DeleteNotes();
    void AppearNotes();// ノーツを出現させる

private:
    float duration_;// 譜面の長さ
    float currentTime_;// 現在の時間
    std::vector<std::pair<float, std::shared_ptr<Note_Base>>> notes_;// すべてのノーツ
    std::vector<std::weak_ptr<Note_Base>> activeHoldNotes_;// アクティブなホールドノーツ
    std::list<std::weak_ptr<Note_Base>> onFieldNotes_;// フィールド上のノーツ(見えているノーツ)
    
};