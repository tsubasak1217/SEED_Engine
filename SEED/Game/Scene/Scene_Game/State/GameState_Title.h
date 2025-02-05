#pragma once

// state
#include "State_Base.h"
#include "Scene_Game.h"
#include "Sprite.h"  // フェード用のSpriteを利用するために追加

// stage
#include "Stage.h"

// lib
#include <memory>

class GameState_Title : public State_Base{

public:
    GameState_Title() = default;
    GameState_Title(Scene_Base* pScene);
    ~GameState_Title() override;
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;
    void ManageState() override;

private:
    Scene_Game* pGameScene_;
    Stage* stage_;

    Vector3 cameraOffset_;
    Vector3 cameraRotate_;
    Vector3 playerRotate_;

    std::unique_ptr<Sprite> titleLogo_ = nullptr;

    // FadeIn用のメンバー変数
    std::unique_ptr<Sprite> fade_;
    bool fadeInStarted_ = true;
    const float fadeSpeed_ = 0.05f;
};
