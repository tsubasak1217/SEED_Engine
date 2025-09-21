#pragma once
#include <memory>
#include <Game/Objects/Judgement/PlayField.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <Game/Objects/Notes/NotesData.h>
#include <SEED/Source/Basic/Camera/BaseCamera.h>
#include <Game/Objects/Combo/ComboObject.h>
#include <Game/Manager/NotesEditor.h>
#include <Game/Objects/Result/PlayResult.h>

class RythmGameManager{
private:
    RythmGameManager();
    RythmGameManager(const RythmGameManager&) = delete;
    void operator=(const RythmGameManager&) = delete;
    static RythmGameManager* instance_;

public:
    ~RythmGameManager();
    static RythmGameManager* GetInstance();
    void Initialize(const nlohmann::json& songData);
    void BeginFrame();
    void EndFrame();
    void Update();
    void Draw();

public:
    BaseCamera* GetCamera(){ return gameCamera_.get(); }
    void Pause();
    void Resume();
    void BreakCombo(){ comboObject_->comboCount = 0; }
    void AddCombo(){ 
        comboObject_->comboCount++; 
        comboObject_->comboCount > playResult_.maxCombo ? playResult_.maxCombo = comboObject_->comboCount : playResult_.maxCombo;
    }

    void AddEvaluation(Judgement::Evaluation evalution){
        playResult_.evalutionCount[(int)evalution]++;
    }

    // fast, lateのカウントを追加
    void AddFastCount(int count = 1){
        playResult_.fastCount += count;
    }

    void AddLateCount(int count = 1){
        playResult_.lateCount += count;
    }

private:
    float CalculateScore();

private:
    std::unique_ptr<BaseCamera> gameCamera_;// カメラ
    std::unique_ptr<NotesData> notesData_;// 譜面データ
    std::unique_ptr<ComboObject> comboObject_; // コンボオブジェクト
    std::unique_ptr<NotesEditor> notesEditor_; // ノーツエディタ
    PlayResult playResult_; // プレイ結果
    bool isPaused_ = false; // ポーズ中かどうか
};
