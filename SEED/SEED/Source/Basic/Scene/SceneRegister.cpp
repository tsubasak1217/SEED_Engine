#include "SceneRegister.h"
// 各シーンのヘッダーファイル
#include <Game/Scene/Scene_Game/Scene_Game.h>

void SceneRegister::RegisterScenes() {
    SceneManager::Register("Game", []() {return new Scene_Game(); });
}