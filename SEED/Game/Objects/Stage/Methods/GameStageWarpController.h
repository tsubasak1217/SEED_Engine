#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>

// c++
#include <vector>
#include <ranges>
#include <array>
#include <span>
// json
#include <SEED/External/nlohmann/json.hpp>
// front
class Player;
class Warp;

//============================================================================
//	GameStageWarpController class
//============================================================================
class GameStageWarpController {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    GameStageWarpController() = default;
    ~GameStageWarpController() = default;

    // 初期化処理
    void Initialize();

    // 更新処理
    void Update();

    // エディター
    void Edit();

    // json
    void FromJson(const nlohmann::json& data);
    void ToJson(nlohmann::json& data);

    // リセット
    void ResetWarps(StageObjectCommonState state);

    // 対象のワープの相手がいるかどうかチェック
    bool CheckWarpPartner(StageObjectCommonState state, uint32_t warpIndex);
    
    // 死亡時のワープ処理
    void DeadWarp(const Vector2& start, const Vector2& target);

    //--------- accessor -----------------------------------------------------

    // プレイヤーのセット
    void SetPlayer(Player* player) { player_ = player; }

    // ワープのセット
    void SetWarps(StageObjectCommonState state, const std::vector<Warp*> warps);

    // ワープ中かどうか
    bool IsWarping() const { return currentState_ == State::Warping; }
    bool IsDeadWarping() const { return isDeadWarp_; }

    // ワープ先座標をインデックスから取得する
    Vector2 GetWarpTargetTranslateFromIndex(StageObjectCommonState state, uint32_t warpIndex) const;
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // 状態
    enum class State {

        WarpPossible,    // ワープ可能
        Warping,         // ワープ中...
        WarpNotPossible, // ワープ不可
    };

    //--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;

    // プレイヤー
    Player* player_;
    bool isDeadWarp_;

    // ワープオブジェクト
    std::vector<Warp*> noneWarps_;     // 通常ワープ
    std::vector<Warp*> hologramWarps_; // ホログラムワープ

    // ワープを行う実体
    Warp* executingWarpStart_;  // ワープ開始
    Warp* executingWarpTarget_; // ワープ目標

    //--------- functions ----------------------------------------------------

    // update
    void UpdateWarpPossible();
    void UpdateWarping();
    void UpdateWarpNotPossible();

    // helper
    void ResetWarp();
    bool IsWarpCameNotification();
    bool CheckWarpTarget();
    const std::vector<Warp*>* GetWarpTarget(StageObjectCommonState state) const;
};