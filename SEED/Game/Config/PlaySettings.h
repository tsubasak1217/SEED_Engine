#pragma once
#include <json.hpp>
#include <memory>


/// <summary>
/// 音ゲーのセッティング情報を持つクラス
/// </summary>
class PlaySettings{
private:
    PlaySettings();
    inline static std::unique_ptr<PlaySettings> instance_ = nullptr;
    PlaySettings(const PlaySettings&) = delete;
    PlaySettings& operator=(const PlaySettings&) = delete;

public:
    ~PlaySettings();
    static PlaySettings* GetInstance();

public:
    // ノーツスピード
    float GetNoteSpeedRate() const{ return laneNoteSpeedRate_; }
    void SetNoteSpeedRate(float speed){ laneNoteSpeedRate_ = speed; }
    // ノーツの出現時間
    float GetLaneNoteAppearTime() const{ return kNoteAppearTime * (1.0f/ std::clamp(laneNoteSpeedRate_,0.01f,FLT_MAX)); }
    float GetKNoteAppearTime() const{ return kNoteAppearTime; }
    // 判定のオフセット
    float GetOffsetJudge() const{ return offset_judge_; }
    void SetOffsetJudge(float offset){ offset_judge_ = offset; }
    float GetOffsetView() const{ return offset_view_; }
    void SetOffsetView(float offset){ offset_view_ = offset; }
    float GetTotalOffsetAnswerSE() const{ return baseOffset_answerSE_ + offset_answerSE_; }
    float GetOffsetAnswerSE() const{ return offset_answerSE_; }
    void SetOffsetAnswerSE(float offset){ offset_answerSE_ = offset; }
    // 反転設定
    bool GetIsReverseLR() const{ return isReverseLR_; }
    void SetIsReverseLR(bool isReverse){ isReverseLR_ = isReverse; }
    bool GetIsReverseUD() const{ return isReverseUD_; }
    void SetIsReverseUD(bool isReverse){ isReverseUD_ = isReverse; }
    // カーソル感度
    float GetTotalCursorSenstivity() const{ return baseCursorSensitivity_ * cursorSenstivity_; }
    float GetCursorSenstivity() const{ return cursorSenstivity_; }
    void SetCursorSenstivity(float sensitivity){ cursorSenstivity_ = sensitivity; }

#ifdef _DEBUG
    void Edit();
#endif // _DEBUG


private:
    // ノーツスピード
    float kNoteAppearTime = 4.0f;// レーン上にノーツが出現している時間(奥から手前に達するまでの時間)
    float laneNoteSpeedRate_ = 1.0f;// レーン上のノーツのスピード割合

    //判定のオフセット
    float offset_judge_ = 0.0f;// 判定そのもののオフセット
    float offset_view_ = 0.0f;// 表示のオフセット
    float offset_answerSE_ = 0.0f;// アンサー音のオフセット
    float baseOffset_answerSE_ = -0.16f;// 基本アンサー音オフセット
    // カーソル設定
    float baseCursorSensitivity_ = 0.5f;// 基本カーソル感度
    float cursorSenstivity_ = 1.0f;// カーソルの感度
    // 反転設定
    bool isReverseLR_ = false;// 左右反転
    bool isReverseUD_ = false;// 上下反転
};