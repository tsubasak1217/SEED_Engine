#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Player/State/Interface/PlayerIState.h>

//============================================================================
//	PlayerDeadState class
//============================================================================
class PlayerDeadState : public PlayerIState {
public:
    //========================================================================
    //	public Methods
    //========================================================================
    PlayerDeadState() = default;
    ~PlayerDeadState() = default;
    // 状態遷移時
    void Enter(Player& player) override;
    // 更新処理
    void Update(Player& player) override;
    // 状態終了時
    void Exit(Player& player) override;
    // エディター
    void Edit(const Player& player) override;
    // json
    void FromJson(const nlohmann::json& data) override;
    void ToJson(nlohmann::json& data) override;


    //--------- accessor -----------------------------------------------------

    bool IsDead() const { return isDead_; }

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    bool isDead_; // 死んだかどうか
};

