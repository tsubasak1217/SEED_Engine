#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>

// c++
#include <vector>
#include <optional>
#include <unordered_set>
#include <unordered_map>
// json
#include <SEED/External/nlohmann/json.hpp>
// front
class LaserLauncher;
class GameStageWarpController;
class GameObject2D;

//============================================================================
//	GameStageLaserController class
//============================================================================
class GameStageLaserController {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameStageLaserController() = default;
	~GameStageLaserController() = default;

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
    void ResetLauncheres(StageObjectCommonState state);
    void ResetWarpIgnore();

	//--------- accessor -----------------------------------------------------

    // ワープ管理クラスの取得
    void SetWarpController(GameStageWarpController* warpController) { warpController_ = warpController; }

    // 発射台のセット
    void SetLaserLauncheres(StageObjectCommonState state, const std::vector<LaserLauncher*> launcheres);
private:
	//========================================================================
	//	private Methods
	//========================================================================

    //--------- structure ----------------------------------------------------

    // ワープによるレーザー発生
    struct WarpSpawn {

        LaserLauncher* launcher;         // 発射台
        Vector2 target;                  // ワープ先目標
        GameObject2D* sourceLaserObject; // 発生元レーザー
    };

	//--------- variables ----------------------------------------------------

    // ワープ管理クラスを取得する
    GameStageWarpController* warpController_;

    // レーザー発射台
    std::vector<LaserLauncher*> noneLaserLauncheres_;     // 通常発射台
    std::vector<LaserLauncher*> hologramLaserLauncheres_; // ホログラム発射台

    // ワープ目標先
    std::vector<std::optional<Vector2>> noneLaserLauncherWarpTargets_;     // 通常
    std::vector<std::optional<Vector2>> hologramLaserLauncherWarpTargets_; // ホログラム

    std::vector<WarpSpawn> pendingSpawns_;
    // ワープと衝突しているレーザー
    std::unordered_set<GameObject2D*> inWarpLasers_;
    // 使用したワープのオブジェクトインデックスマップ
    std::unordered_map<uint64_t, std::unordered_set<uint32_t>> usedWarpIndexByFamily_;

	//--------- functions ----------------------------------------------------

    // update
    void CheckWarpLasers();
    void UpdateLaserLauncheres();
};