#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Enum/StageObjectType.h>

// c++
#include <vector>
// front
class Player;
class Warp;

//============================================================================
//	GameStageWarpManager class
//============================================================================
class GameStageWarpManager {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    GameStageWarpManager() = default;
    ~GameStageWarpManager() = default;

    //--------- accessor -----------------------------------------------------

    void SetWarps(StageObjectCommonState state, const std::vector<Warp*> warps);
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    // プレイヤー
    Player* player_;

    // 通常ワープ
    std::vector<Warp*> noneWarps_;
    // ホログラムワープ
    std::vector<Warp*> hologramWarps_;

    //--------- functions ----------------------------------------------------

};