#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Player/State/Interface/PlayerIState.h>

//============================================================================
//	PlayerJumpState class
//============================================================================
class PlayerJumpState :
    public PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerJumpState() = default;
	~PlayerJumpState() = default;

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

    bool IsJumping() const { return isJumping_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    bool isJumping_;  // ジャンプ中かどうか
    bool isFalling_;  // 落下中かどうか
    float velocityY_; // 現在のY方向速度

    float jumpPower_; // ジャンプ力
    float gravity_;   // 重力
    float groundY_;   // 着地点のY座標
};