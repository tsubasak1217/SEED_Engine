#pragma once
#include <Game/Objects/Notes/Note_Base.h>

/// <summary>
/// サイドフリックノーツ
/// </summary>
class Note_SideFlick : public Note_Base{
public:
    Note_SideFlick();
    ~Note_SideFlick() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evaluation Judge(float curTime) override;

    // 入出力関数
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;

#ifdef _DEBUG
    void Edit()override;
#endif // _DEBUG

public:
    LR flickDirection_ = LR::NONE;// フリックの方向
};