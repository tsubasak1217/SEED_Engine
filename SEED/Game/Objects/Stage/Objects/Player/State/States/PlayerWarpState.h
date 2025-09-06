#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Functions/myFunc/Easing.h>
#include <Game/Objects/Stage/Objects/Player/State/Interface/PlayerIState.h>

//============================================================================
//	PlayerWarpState class
//============================================================================
class PlayerWarpState :
    public PlayerIState {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    PlayerWarpState() = default;
    ~PlayerWarpState() = default;

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

    void SetLerpValue(const Vector2& start, const Vector2& target);
    void ResetWarpFinishTrigger() { isWarpFinishTrigger_ = false; }

    bool IsWarping() const { return isWarping_; }
    bool IsWarpFinishTrigger() const { return isWarpFinishTrigger_; }
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // 状態
    enum class State {

        Begin, // ワープ開始アニメーション
        Warp,  // ワープ中
        End,   // ワープ終了アニメーション
    };

    // アニメーションパラメータ
    template <typename T>
    struct AnimParam {

        T start;
        T target;
        Easing::Type easing;
    };

    //--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;

    bool isWarpFinishTrigger_; // ワープ終了判定
    bool isWarping_;           // ワープ中かどうか

    // 補間座標
    Vector2 warpStart_;  // 開始
    Vector2 warpTarget_; // 目標

    // パラメータ
    // BeginState
    // アニメーション
    Timer beginTimer_;
    AnimParam<float> beginScaling_;     // 開始スケーリング
    AnimParam<Vector2> beginTranslate_; // 開始座標
    // WarpState
    Timer warpTimer_; // ワープにかかる時間
    // EndState
    // アニメーション
    Timer endTimer_;
    AnimParam<float> endScaling_; // 終了スケーリング

    // エディター
    State editState_ = State::Begin;

    //--------- functions ----------------------------------------------------

    // update
    void UpdateBeginAnimation(Player& player);
    void UpdateWarp(Player& player);
    void UpdateEndAdnimation(Player& player);
};