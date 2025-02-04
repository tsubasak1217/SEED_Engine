#include "SceneRegister.h"

// 各シーンのヘッダーファイル
#include "Scene_Game.h"
#include "Scene_Title.h"
#include "Scene_Clear.h"

void SceneRegister::RegisterScenes() {
    SceneManager::Register("Title", []() {return new Scene_Title(); });
    SceneManager::Register("Game", []() {return new Scene_Game(); });
    SceneManager::Register("Clear",[](){return new Scene_Clear(); });
}
