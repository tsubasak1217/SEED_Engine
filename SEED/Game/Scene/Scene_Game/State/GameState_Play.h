#pragma once
#include <SEED/Source/Basic/Scene/SceneState_Base.h>
#include <Game/Manager/RhythmGameManager.h>
#include <json.hpp>

/// <summary>
/// 音ゲーをプレイするステート
/// </summary>
class GameState_Play : public SEED::SceneState_Base{
public:
    GameState_Play() = default;
    GameState_Play(SEED::Scene_Base* pScene);
    GameState_Play(SEED::Scene_Base* pScene, const SongInfo& songInfo,int32_t difficulty);
    ~GameState_Play() override;

public:
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;
    void ManageState() override;

private:

};