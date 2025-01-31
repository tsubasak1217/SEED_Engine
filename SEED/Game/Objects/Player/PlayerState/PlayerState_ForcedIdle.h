#pragma once
#include "PlayerState_Idle.h"

/// <summary>
/// 強制アイドル状態(外部からのみステートを変更できる)
/// </summary>
class PlayerState_ForcedIdle : public PlayerState_Idle
{
public:
    PlayerState_ForcedIdle(BaseCharacter *_pCharacter);
    ~PlayerState_ForcedIdle();

    void Initialize(const std::string &stateName, BaseCharacter *player) override;
    void Update() override;
    void Draw() override;

protected:
    void ManageState() override;
};