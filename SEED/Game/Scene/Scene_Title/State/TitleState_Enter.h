#pragma once

#include "ITitleState.h"

//objec
#include "../UI/UI.h"

class TitleState_Enter
    : public ITitleState{
public:
    TitleState_Enter(Scene_Title* _host);
    ~TitleState_Enter();

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