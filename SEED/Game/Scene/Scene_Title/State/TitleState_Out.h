#pragma once

#include "ITitleState.h"

//object
class UI;

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
    std::unique_ptr<UI> fadeOutScreen_;
};