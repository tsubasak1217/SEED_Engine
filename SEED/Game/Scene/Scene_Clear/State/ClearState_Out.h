#pragma once
#include "State_Base.h"

#include <Sprite.h>

class ClearState_Out :
    public State_Base{
public:
    ClearState_Out(Scene_Base* _host);
    ~ClearState_Out();

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