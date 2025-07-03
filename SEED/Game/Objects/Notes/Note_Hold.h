#pragma once
#include <Game/Objects/Notes/Note_Base.h>

class Note_Hold : public Note_Base{
public:
    Note_Hold();
    ~Note_Hold() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evaluation Judge(float dif) override;
    Judgement::Evaluation JudgeHoldEnd();

    // 入出力関数
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;

public:
    float kHoldTime_;
    float releaseTime_ = 0.0f;
    bool isHold_ = false;// ホールドしているかどうか
    bool isReleased_ = false;// リリースしたかどうか
    bool isStackedToHoldList_ = false;// スタックリストに積まれているか
    Judgement::Evaluation headEvaluation_ = Judgement::Evaluation::NONE;// 頭の判定
    int32_t textureGHs_[3];// ホールドノーツのテクスチャ
    Vector4 noteColors_[3];// ホールドノーツの色

#ifdef _DEBUG
    void Edit()override;
    bool isDraggingHoldStart_ = false;
    bool isDraggingHoldEnd_ = false;
#endif // _DEBUG

};