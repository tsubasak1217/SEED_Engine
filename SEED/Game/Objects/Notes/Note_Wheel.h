#pragma once
#include <Game/Objects/Notes/Note_Base.h>

/// <summary>
/// ホイールノーツ
/// </summary>
class Note_Wheel : public Note_Base{
public:
    Note_Wheel();
    ~Note_Wheel() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evalution Judge(float curTime) override;

    // 入出力関数
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;

#ifdef _DEBUG
    void Edit()override;
#endif // _DEBUG

public:
    UpDown direction_ = UpDown::NONE;
    uint32_t wheelDirectionGH_;
    uint32_t wheelAuraGH_;
    Vector4 upDownColor_[2];
    Vector3 uv_translate_;
};