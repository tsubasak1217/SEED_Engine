#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerIState.h>
#include <Game/Objects/Player/State/Enum/PlayerStateEnum.h>

// c++
#include <unordered_map>
// front

//============================================================================
//	PlayerStateController class
//============================================================================
class PlayerStateController {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerStateController() = default;
	~PlayerStateController() = default;

    // 初期化処理
    void Initialize();

    // 更新処理
    void Update(Player& owner);

    // エディター
    void Edit(const Player& owner);

    // json
    void FromJson(const nlohmann::json& data);
    void ToJson(nlohmann::json& data);

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    PlayerState current_;                  // 現在の状態
    std::optional<PlayerState> requested_; // 次の状態

    // 入力管理
    std::unique_ptr<InputMapper<PlayerInputAction>> inputMapper_;

    // 全ての状態
    std::unordered_map<PlayerState, std::unique_ptr<PlayerIState>> states_;

    // エディター用
    PlayerState editState_;

	//--------- functions ----------------------------------------------------

    // update
    void UpdateInputState();

    // helper
    void Request(PlayerState state);
    void ChangeState(Player& owner);
    void CheckJumpState(Player& owner);
};