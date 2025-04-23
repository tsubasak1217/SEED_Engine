#pragma once
#include <vector>
#include <queue>
#include <Game/Objects/Notes/Note_Base.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>

class NotesData{
public:
    NotesData();
    ~NotesData();
    void Initialize();
    void Update();
    void Draw();

public:
    void AddNote();
    void RemoveNote();
    void LoadFromFile(const std::string& fileName);
    std::vector<std::weak_ptr<Note_Base>> GetNearNotes(float time);
    float GetDuration(){ return duration_; }

private:
    void DeleteNotes();

private:
    float duration_;// 譜面の長さ
    float currentTime_;// 現在の時間
    std::vector<std::pair<float, std::shared_ptr<Note_Base>>> notes_;// すべてのノーツ
    std::vector<std::weak_ptr<Note_Base>> activeHoldNotes_;// アクティブなホールドノーツ
};