#pragma once
#include <Game/Objects/Notes/Note_Base.h>

/// <summary>
/// ホールドノーツ
/// </summary>
class Note_Hold : public Note_Base{
public:
    Note_Hold();
    ~Note_Hold() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evalution Judge(float curTime) override;
    Judgement::Evalution JudgeHoldEnd();

    // 入出力関数
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;

public:
    float kHoldTime_;
    float releaseTime_ = 0.0f;
    float hitTime_;// 判定時間
    bool isHold_ = false;// ホールドしているかどうか
    bool isReleased_ = false;// リリースしたかどうか
    bool isStackedToHoldList_ = false;// スタックリストに積まれているか
    Judgement::Evalution headEvaluation_ = Judgement::Evalution::NONE;// 頭の判定
    int32_t textureGHs_[3];// ホールドノーツのテクスチャ
    Vector4 noteColors_[3];// ホールドノーツの色

#ifdef _DEBUG
    void Edit()override;
    bool isDraggingHoldStart_ = false;
    bool isDraggingHoldEnd_ = false;
#endif // _DEBUG

};