#pragma once
// external
#include <string>
#include <vector>
#include <memory>
#include <list>

// local
#include <Triangle.h>
#include <Model.h>
#include <State_Base.h>
#include <Scene_Base.h>

// editor
#include "../Game/Editor/FieldEditor.h"
#include "CollisionManaer/ColliderEditor.h"
#include "../Game/Editor/EnemyEditor.h"

class Scene_Game;

class GameState_Play : public State_Base{
public:
    GameState_Play() = default;
    GameState_Play(Scene_Base* pScene);
    ~GameState_Play();
    void Update()override;
    void Draw()override;
    void Initialize()override;
    void Finalize()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;

private:
    Scene_Game* pGameScene_;

private:
    std::unique_ptr<ColliderEditor> fieldColliderEditor_;
    std::unique_ptr<FieldEditor> fieldEditor_;
    std::unique_ptr<EnemyEditor> enemyEditor_;
};