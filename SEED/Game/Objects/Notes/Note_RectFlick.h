#pragma once
#include <Game/Objects/Notes/Note_Base.h>

/// <summary>
/// ㇾクトフリック
/// </summary>
class Note_RectFlick : public Note_Base{
public:
    Note_RectFlick();
    ~Note_RectFlick() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evaluation Judge(float curTime) override;

    // 入出力関数
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;

#ifdef _DEBUG
    void Edit()override;
    std::string bitName;
#endif // _DEBUG

private:
    bool CheckBit(DIRECTION8 dir) const;
};