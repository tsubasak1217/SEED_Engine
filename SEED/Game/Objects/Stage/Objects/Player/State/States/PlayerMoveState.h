#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Player/State/Interface/PlayerIState.h>

//============================================================================
//	PlayerMoveState class
//============================================================================
class PlayerMoveState :
    public PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerMoveState() = default;
	~PlayerMoveState() = default;

    // 状態遷移時
    void Enter(Player& player) override;

    // 更新処理
    void Update(Player& player) override;

    // 状態終了時
    void Exit(Player& player) override;

    // アクセッサ
    bool GetIsMoving() const;

    // エディター
    void Edit(const Player& player) override;

    // json
    void FromJson(const nlohmann::json& data) override;
    void ToJson(nlohmann::json& data) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    float moveSpeed_; // 移動速度

    // audio
    uint32_t moveSE_;
};