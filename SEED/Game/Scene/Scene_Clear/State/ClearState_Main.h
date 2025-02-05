#pragma once
#include "State_Base.h"

//host
class Scene_Clear;

//object
#include "Model.h"

//lib
class Input;

class ClearState_Main :
    public State_Base{
public:
    ClearState_Main(Scene_Clear* scene);
    ~ClearState_Main();

    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Finalize()override;

    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
    void ManageState()override;
private:
    void ChangeModel();

private:
    Scene_Clear* pClearScene_;

    bool isBreakEgg_ = false;
};
