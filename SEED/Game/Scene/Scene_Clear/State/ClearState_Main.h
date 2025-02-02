#pragma once
#include "State_Base.h"

class Input;

class ClearState_Main :
    public State_Base{
public:
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Finalize()override;

    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;
private:
};
