#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Objects/Player/State/Interface/PlayerIState.h>
#include <Game/Objects/Stage/Objects/Player/State/Enum/PlayerStateEnum.h>

// c++
#include <unordered_map>

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
    void Initialize(const InputMapper<PlayerInputAction>* inputMapper);

    // 更新処理
    void Update(Player& owner);

    // エディター
    void Edit(const Player& owner);

    // json
    void FromJson(const nlohmann::json& data);
    void ToJson(nlohmann::json& data);

    //--------- accessor -----------------------------------------------------

    // ワープ状態にする
    void SetWarpState(const Vector2& start, const Vector2& target);
    // ワープ状態じゃなくなったか
    bool IsFinishedWarp() const;

    // 境界線を操作できる状態かどうか
    bool IsCanOperateBorder() const;

    // 着地した瞬間の処理
    void OnGroundTrigger();
    // 天井に当たった瞬間の処理
    void OnCeilingTrigger();

    float GetJumpVelocity() const;

    // 現在の状態を取得
    PlayerState GetCurrentState() const{ return current_; }
    bool GetIsMoving()const;

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    PlayerState current_;                  // 現在の状態
    PlayerState pre_;                      // 前回の状態
    std::optional<PlayerState> requested_; // 次の状態
    bool requestedJump_;                   // ジャンプをリクエストしたか

    // 入力管理
    const InputMapper<PlayerInputAction>* inputMapper_;
    // 全ての状態
    std::unordered_map<PlayerState, std::unique_ptr<PlayerIState>> states_;

    // エディター用
    PlayerState editState_;

    //--------- functions ----------------------------------------------------

    // update
    void UpdateInputState();
    void CheckOwnerState(Player& owner);

    // helper
    void Request(PlayerState state);
    void ChangeState(Player& owner);
    void CheckJumpState(Player& owner);
    void CheckWarpState(Player& owner);
};