#pragma once

#include "ITitleState.h"

//object
#include "Sprite.h"

class TitleState_Out
    : public ITitleState{
public:
    TitleState_Out(Scene_Title* _host);
    ~TitleState_Out();

    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Finalize()override;

    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;
private:
    std::unique_ptr<Sprite> whiteScreen_ = nullptr;

    float fadeTimer_ = 0.f;
    float fadeTime_ = 0.6f;
};