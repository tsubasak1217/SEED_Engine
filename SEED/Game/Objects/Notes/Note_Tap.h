#pragma once
#include <Game/Objects/Notes/Note_Base.h>

/// <summary>
/// タップノーツ
/// </summary>
class Note_Tap : public Note_Base{
public:
    Note_Tap();
    ~Note_Tap() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evalution Judge(float curTime) override;

    // 入出力関数
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;

#ifdef _DEBUG
    void Edit()override;
#endif // _DEBUG

};