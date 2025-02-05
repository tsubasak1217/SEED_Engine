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

// engine
#include "Sprite.h"

// editor
#include "../Game/Editor/FieldEditor.h"
#include "CollisionManaer/ColliderEditor.h"
#include "../Game/Editor/EnemyEditor.h"

class Scene_Game;

class GameState_Play : public State_Base{
public:
    GameState_Play() = default;
    GameState_Play(Scene_Base* pScene,bool isPlayerSetStartPos = true);
    ~GameState_Play();
    void Update()override;
    void Draw()override;
    void Initialize(bool isPlayerSetStartPos);
    void Finalize()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;
private:
    void FadeUpdate();

public:
    FieldEditor* GetFieldEditor(){ return fieldEditor_.get(); }

private:
    Scene_Game* pGameScene_;

    // sprite
    std::unique_ptr<Sprite> pauseButton = nullptr;

    bool isFadeIn_ = false;
    bool isFadeOut_ = false;
    float currentTime_ = 0.0f;
   const float fadeInTime_ = 1.5f;
   const float fadeOutTime_ = 1.4f;
    std::unique_ptr<Sprite> fade_ = nullptr;

private:
    std::unique_ptr<ColliderEditor> fieldColliderEditor_;
    std::unique_ptr<FieldEditor> fieldEditor_;

private:
};