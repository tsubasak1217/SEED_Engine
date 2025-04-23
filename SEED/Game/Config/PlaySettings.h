#pragma once

class PlaySettings{
private:
    PlaySettings();
    inline static PlaySettings* instance_ = nullptr;
    PlaySettings(const PlaySettings&) = delete;
    PlaySettings& operator=(const PlaySettings&) = delete;

public:
    ~PlaySettings() = default;
    static PlaySettings* GetInstance(){
        if(instance_ == nullptr){
            instance_ = new PlaySettings();
        }
        return instance_;
    }

public:
    // ノーツスピード
    float GetNoteSpeed() const{ return noteSpeed_; }
    void SetNoteSpeed(float speed){ noteSpeed_ = speed; }
    // 判定のオフセット
    float GetOffsetJudge() const{ return offset_judge_; }
    void SetOffsetJudge(float offset){ offset_judge_ = offset; }
    float GetOffsetView() const{ return offset_view_; }
    void SetOffsetView(float offset){ offset_view_ = offset; }
    // 反転設定
    bool GetIsReverseLR() const{ return isReverseLR_; }
    void SetIsReverseLR(bool isReverse){ isReverseLR_ = isReverse; }
    bool GetIsReverseUD() const{ return isReverseUD_; }
    void SetIsReverseUD(bool isReverse){ isReverseUD_ = isReverse; }

private:
    // ノーツスピード
    const float kNoteAppearTime = 2.0f;// ノーツが出現している時間(奥から手前に達するまでの時間)
    float noteSpeed_ = 1.0f;// ノーツのスピード(1.0f = 1レーン分の距離をkNoteAppearTime秒で移動する)
    //判定のオフセット
    float offset_judge_ = 0.0f;// 判定そのもののオフセット
    float offset_view_ = 0.0f;// 表示のオフセット
    // 反転設定
    bool isReverseLR_ = false;// 左右反転
    bool isReverseUD_ = false;// 上下反転
};