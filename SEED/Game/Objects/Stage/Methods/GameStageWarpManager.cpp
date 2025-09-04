#include "GameStageWarpManager.h"

//============================================================================
//	GameStageWarpManager classMethods
//============================================================================

void GameStageWarpManager::SetWarps(StageObjectCommonState state,
    const std::vector<Warp*> warps) {

    switch (state) {
    case StageObjectCommonState::None: {

        noneWarps_.clear();
        noneWarps_ = warps;
        break;
    }
    case StageObjectCommonState::Hologram: {

        hologramWarps_.clear();
        hologramWarps_ = warps;
        break;
    }
    }
}