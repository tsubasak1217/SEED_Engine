#pragma once

#include "ITitleState.h"

class TitleState_Main
    : public ITitleState{
public:
    TitleState_Main(Scene_Title* _host);
    ~TitleState_Main();

    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Finalize()override;

    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;
};

