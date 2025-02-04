#pragma once
#include "State_Base.h"

//objec
#include "Sprite.h"

class GameState_Enter
    : public State_Base{
public:
    GameState_Enter(Scene_Base* _host);
    ~GameState_Enter();

    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Finalize()override;

    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;
private:
    // フェード用
    std::unique_ptr<Sprite> whiteScreen_ = nullptr;
    float fadeTime_ = 0.6f;
    float fadeTimer_ = 0.f;
};