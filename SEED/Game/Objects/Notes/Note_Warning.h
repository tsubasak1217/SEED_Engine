#pragma once
#include <Game/Objects/Notes/Note_Base.h>
#include <SEED/Lib/Structs/Timer.h>

/// <summary>
/// 警告ノーツ
/// </summary>
class Note_Warning : public Note_Base{
public:
    Note_Warning();
    ~Note_Warning() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evaluation Judge(float curTime) override;

    // 入出力関数
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;

#ifdef _DEBUG
    void Edit()override;
#endif // _DEBUG

    float duration_ = 0.0f;
    Timer timer_;
    bool start_ = false;
    bool isEmitWarningObj_ = true;
};