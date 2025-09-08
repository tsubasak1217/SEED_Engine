#include "SceneRegister.h"
// 各シーンのヘッダーファイル
#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <Game/Scene/Scene_Title/Scene_Title.h>
#include <Game/Scene/Scene_Select/Scene_Select.h>

void SceneRegister::RegisterScenes() {
    SceneManager::Register("Title", []() {return new Scene_Title(); });
    SceneManager::Register("Game", []() {return new Scene_Game(); });
    SceneManager::Register("Select", []() {return new Scene_Select(); });
}